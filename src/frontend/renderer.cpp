// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by kaichiuchu <kaichiuchu@protonmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "renderer.h"

#include "models/app_settings.h"

namespace {
GLuint vertex_shader;
GLuint fragment_shader;
};  // namespace

Renderer::Renderer(QWidget* parent_widget) noexcept
    : QOpenGLWidget(parent_widget) {}

void Renderer::initializeGL() noexcept {
  initializeOpenGLFunctions();
  CreateVertexShader();
  CreateFragmentShader();
  CreateProgram();
  DestroyShaders();
  CreateVertexArray();
  CreateElementArray();
  CreateTexture();
  SetupFromAppSettings();
}

void Renderer::resizeGL(int w, int h) noexcept { glViewport(0, 0, w, h); }

void Renderer::paintGL() noexcept {
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScreen(
    const chip8::ImplementationInterface::Framebuffer& framebuffer) noexcept {
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, chip8::framebuffer::kWidth,
               chip8::framebuffer::kHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
               framebuffer.data());
  update();
}

void Renderer::EnableBilinearFiltering(const bool enabled) noexcept {
  const auto filter = enabled ? GL_LINEAR : GL_NEAREST;

  glBindTexture(GL_TEXTURE_2D, texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

auto Renderer::CreateShader(const GLenum type, const char* const src) noexcept
    -> GLuint {
  auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  return shader;
}

void Renderer::CreateTexture() noexcept {
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::SetupFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  EnableBilinearFiltering(app_settings.BilinearFilteringEnabled());
}

void Renderer::CreateVertexArray() noexcept {
  // clang-format off
  constexpr float vertex_data[] = {
    // Positions    Texture coordinates
       1.0F, -1.0F, 1.0F, 1.0F, // Top right
       1.0F,  1.0F, 1.0F, 0.0F, // Bottom right
      -1.0F,  1.0F, 0.0F, 0.0F, // Bottom left
      -1.0F, -1.0F, 0.0F, 1.0F  // Top left
  };
  // clang-format on

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glGenBuffers(1, &vbo_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Renderer::CreateElementArray() noexcept {
  // clang-format off
  constexpr unsigned int indices[] = {
    0, 1, 3, // First triangle
    1, 2, 3  // Second triangle
  };
  // clang-format on

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
}

void Renderer::CreateVertexShader() noexcept {
  constexpr auto vertex_shader_src = R"(
  #version 330 core
  layout (location = 0) in vec3 Position;
  layout (location = 1) in vec2 TexCoord;

  out vec2 Real_TexCoord;

  void main()
  {
    gl_Position = vec4(Position, 1.0);
    Real_TexCoord = TexCoord;
  })";
  vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_src);
}

void Renderer::CreateFragmentShader() noexcept {
  constexpr auto fragment_shader_src = R"(
  #version 330 core
  in vec2 Real_TexCoord;
  out vec4 Color;

  uniform sampler2D MainTexture;

  void main()
  {
    Color = texture(MainTexture, Real_TexCoord);
  })";
  fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_src);
}

void Renderer::CreateProgram() noexcept {
  program_ = glCreateProgram();
  glAttachShader(program_, fragment_shader);
  glAttachShader(program_, vertex_shader);

  glLinkProgram(program_);
  glUseProgram(program_);
}

void Renderer::DestroyShaders() noexcept {
  glDeleteShader(fragment_shader);
  glDeleteShader(vertex_shader);
}