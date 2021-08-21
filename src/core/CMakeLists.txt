# vm-tutorial - Virtual machine tutorial targeting CHIP-8
#
# Written in 2021 by kaichiuchu <kaichiuchu@protonmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.

# I shouldn't have to say this, but if I have to I will: for obvious reasons
# we'll always want to compile the core...
add_subdirectory(src)

# XXX: Add a CMake variable to control whether or not the tests are built. We
# are in development full swing, so I don't see any harm in enabling the
# building of tests by default.
add_subdirectory(tests)