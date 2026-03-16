# ZenOS
![2](https://github.com/user-attachments/assets/47cff0b6-14a5-44d6-9dda-248524830bcb)


- [RU README](https://github.com/Junsious/zen-kernel-OS/blob/main/README_ru.md)


## If you have ideas, don’t be shy and write them, create threads, ask to be contributors, we will be only too glad to see new people in this project

## Description


  
- `ZenOS` is an educational project aimed at creating an operating system from scratch. The project includes a kernel written in both C and Rust, with basic functionality for text output and command input support. This project is intended to provide hands-on experience in operating system development, cross-compilation, and low-level programming. The project will also develop.

## Building and Running

### Requirements

- **Cross-compiler**: `i386-elf-gcc` and `i386-elf-as`
- **ISO Image Creation Tool**: `grub-mkrescue`
- **Emulator**: `QEMU`

### Build

1. **Clone the repository**:

    ```bash
    git clone https://github.com/yourusername/ZenOS.git
    cd zen-kernel-OS
    ```

2. **Build the project**:

    ```bash
    make
    ```

3. **Run the ISO image with QEMU**:

    ```bash
    qemu-system-i386 -cdrom ZenOS.iso
    ```

### Project Structure

- **`kernel.c`** — Kernel source code in C.
- **`boot.asm`** — Assembler code for the bootloader.
- **`linker.ld`** — Linker script used for building the kernel.
- **`grub.cfg`** — GRUB configuration file for loading the kernel.
- **`Makefile`** — Build automation file.
- **`keyboard_driver.c`** — Driver for your keyboard on C with special characters.



## Contact

If you have any questions or suggestions, you can reach out to the project maintainer through GitHub or via email: alexandrd542@gmail.com
