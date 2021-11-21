// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by Michael Rodriguez aka kaichiuchu <mike@kaichiuchu.dev>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#pragma once

#include <core/impl.h>

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>

/// This class handles OpenGL rendering for the CHIP-8 framebuffer. It is
/// displayed as the central widget of the main window.
class Renderer : public QOpenGLWidget, public QOpenGLFunctions_4_1_Core {
  Q_OBJECT

 public:
  /// Constructs the renderer.
  ///
  /// \param parent_widget The parent widget of which this class is a child of
  /// it.
  explicit Renderer(QWidget* parent_widget) noexcept;

  /// Updates the screen with new framebuffer data.
  ///
  /// \param framebuffer The framebuffer data to display.
  void UpdateScreen(
      const chip8::ImplementationInterface::Framebuffer& framebuffer) noexcept;

  /// Controls the state of bilinear filtering.
  ///
  /// \param enabled \p true to enable bilinear filtering, or \p false
  /// otherwise.
  void EnableBilinearFiltering(bool enabled) noexcept;

 protected:
  /// From Qt documentation:
  ///
  /// Called once before the first call to \ref QOpenGLWidget::paintGL() or \ref
  /// QOpenGLWidget::resizeGL().
  ///
  /// This method should set up any required OpenGL resources and state.
  ///
  /// There is no need to call \ref QOpenGLWidget::makeCurrent() because this
  /// has already been done when this method is called. Note however that the
  /// framebuffer is not yet available at this stage, so avoid issuing draw
  /// calls from here. Defer such calls to \ref QOpenGLWidget::paintGL()
  /// instead.
  ///
  /// We override this method to initialize the context for our purposes.
  void initializeGL() noexcept override;

  /// From Qt documentation:
  ///
  /// Called whenever the widget has been resized.
  /// The new size is passed in \p w and \p h.
  ///
  /// There is no need to call \ref QOpenGLWidget::makeCurrent() because this
  /// has already been done when this method is called. Additionally, the
  /// framebuffer is also bound.
  ///
  /// We override this method to handle adjusting the viewport.
  ///
  /// \param w The new width of the viewport.
  /// \param h The new height of the viewport.
  void resizeGL(int w, int h) noexcept override;

  /// From Qt documentation:
  ///
  /// Called whenever the widget needs to be painted.
  ///
  /// There is no need to call \ref QOpenGLWidget::makeCurrent() because this
  /// has already been done when this method is called.
  //
  /// Before invoking this method, the context and the framebuffer are bound,
  /// and the viewport is set up by a call to \p glViewport(). No other state is
  /// set and no clearing or drawing is performed.
  ///
  /// We override this method to handle rendering of graphics.
  void paintGL() noexcept override;

 private:
  /// Configures the renderer based on the current application settings.
  void SetupFromAppSettings() noexcept;

  /// Creates the vertex shader.
  void CreateVertexShader() noexcept;

  /// Creates the fragment shader.
  void CreateFragmentShader() noexcept;

  /// Creates the OpenGL program and attaches the vertex and fragment shaders to
  /// it.
  void CreateProgram() noexcept;

  /// Destroys the vertex and fragment shaders.
  ///
  /// This method should only be called once said shaders have been attached to
  /// an OpenGL program.
  void DestroyShaders() noexcept;

  /// Creates the vertex array.
  void CreateVertexArray() noexcept;

  /// Creates the element array.
  void CreateElementArray() noexcept;

  /// Creates the texture that the CHIP-8 framebuffer will be rendered to.
  void CreateTexture() noexcept;

  /// Creates a shader.
  ///
  /// \param type The type of shader to create. Refer to the documentation for
  /// \ref glCreateShader() for valid types.
  ///
  /// \param src The source code for the shader.
  ///
  /// \returns A valid shader object.
  auto CreateShader(GLenum type, const char* const src) noexcept -> GLuint;

  /// The vertex array object created by \ref CreateVertexBufferObject().
  unsigned int vbo_;

  /// The element buffer object.
  unsigned int ebo_;

  /// Current vertex array object.
  unsigned int vao_;

  /// The texture ID to render the CHIP-8 framebuffer on to.
  GLuint texture_;

  /// The current program object.
  GLuint program_;
};