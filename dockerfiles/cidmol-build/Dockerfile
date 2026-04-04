FROM archlinux:latest

RUN pacman -Sy --noconfirm reflector \
  && reflector --latest 5 --sort rate --save /etc/pacman.d/mirrorlist \
  && pacman -Syyu --noconfirm \
  && pacman -S --noconfirm git cmake doxygen clang mold ninja \
  && pacman -Scc --noconfirm