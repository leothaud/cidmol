/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
export module core:elf64;

import :builtins;
import :string;
import :map;

namespace core {

export constexpr u64 EI_NIDENT = 16;

export struct [[gnu::packed]] Elf64Hdr {
  u8 e_ident[EI_NIDENT];
  u16 e_type;
  u16 e_machine;
  u32 e_version;
  u64 e_entry;
  u64 e_phoff;
  u64 e_shoff;
  u32 e_flags;
  u16 e_ehsize;
  u16 e_phentsize;
  u16 e_phnum;
  u16 e_shentsize;
  u16 e_shnum;
  u16 e_shstrndx;
};

export struct [[gnu::packed]] Elf64Phdr {
  u32 p_type;
  u32 p_flags;
  u64 p_offset;
  u64 p_vaddr;
  u64 p_paddr;
  u64 p_filesz;
  u64 p_memsz;
  u64 p_align;
};

export struct [[gnu::packed]] Elf64Shdr {
  u32 sh_name;
  u32 sh_type;
  u64 sh_flags;
  u64 sh_addr;
  u64 sh_offset;
  u64 sh_size;
  u32 sh_link;
  u32 sh_info;
  u64 sh_addralign;
  u64 sh_entsize;
};

export struct [[gnu::packed]] Elf64Sym {
  u32 st_name;
  u8 st_info;
  u8 st_other;
  u16 st_shndx;
  u64 st_value;
  u64 st_size;
};

export enum Elf64Cst {
  PT_NULL = 0,
  PT_LOAD = 1,
  PT_DYNAMIC = 2,
  PT_INTERP = 3,
  PT_NOTE = 4,
  PT_SHLIB = 5,
  PT_PHDR = 6,
  PT_TLS = 7,
  PT_LOOS = 0x60000000,
  PT_HIOS = 0x6fffffff,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7fffffff,
  PT_GNU_EH_FRAME = (PT_LOOS + 0x474e550),
  PT_GNU_STACK = (PT_LOOS + 0x474e551),
  PT_GNU_RELRO = (PT_LOOS + 0x474e552),
  PT_GNU_PROPERTY = (PT_LOOS + 0x474e553)
};

//! Map of all symbols in the executed elf.
export Map<u64, String> symbolNames;

//! Fill the `symbolNames` map.
export void initializeSymbolNames(const char *base) {
  auto *header = reinterpret_cast<const Elf64Hdr *>(base);

  // Check the elf64 magic number
  assert(header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' &&
         header->e_ident[2] == 'L' && header->e_ident[3] == 'F');

  // Find the symtab and strtab
  const Elf64Shdr *sectionHeader =
      reinterpret_cast<const Elf64Shdr *>(base + header->e_shoff);
  const Elf64Shdr *sectionHeaderNames = &sectionHeader[header->e_shstrndx];
  const char *sectionsNames =
      reinterpret_cast<const char *>(base + sectionHeaderNames->sh_offset);
  const Elf64Shdr *symtab = nullptr, *strtab = nullptr;
  for (int i = 0; i < header->e_shnum; ++i) {
    const char *nm = sectionsNames + sectionHeader[i].sh_name;
    if (!symtab && streq(nm, ".symtab")) {
      symtab = &sectionHeader[i];
    }
    if (!strtab && streq(nm, ".strtab")) {
      strtab = &sectionHeader[i];
    }
  }

  // No symbols found?
  if (!symtab || !strtab)
    return;

  const Elf64Sym *symbols =
      reinterpret_cast<const Elf64Sym *>(base + symtab->sh_offset);
  u64 numSymbols = symtab->sh_size / sizeof(Elf64Sym);
  const char *names = reinterpret_cast<const char *>(base + strtab->sh_offset);

  // Fill the map.
  for (u64 i = 0; i < numSymbols; ++i) {
    const char *name = names + symbols[i].st_name;
    u64 address = symbols[i].st_value;
    if (address != 0 && name[0] != '\0')
      symbolNames.insert(address, name);
  }
}

} // namespace core