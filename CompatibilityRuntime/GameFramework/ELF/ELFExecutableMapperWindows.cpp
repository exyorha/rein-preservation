#include <ELF/ELFExecutableMapperWindows.h>
#include <PETypes.h>
#include <ELFPETypes.h>

#include <cstring>

#include <Windows/WindowsError.h>

ELFExecutableMapperWindows::ELFExecutableMapperWindows(const std::filesystem::path &filename) : m_image(filename) {
    auto dosHeader = rva<IMAGE_DOS_HEADER>(0);

    auto elfpeHeaderRVA = dosHeader->e_cp * 512;
    if(dosHeader->e_cblp != 0) {
        elfpeHeaderRVA = elfpeHeaderRVA - 512 + dosHeader->e_cblp;
    }

    auto elfpeHeader = rva<ELFPEHeader>(elfpeHeaderRVA);
    if(elfpeHeader->signature != ELFPEHeader::ELFPESignature)
        throw std::runtime_error("the image doesn't have a valid ELF-PE header");

    m_displacement = reinterpret_cast<uintptr_t>(m_image.base()) + elfpeHeader->elfOffset;
    m_phdr = rva<Elf64_Phdr>(elfpeHeader->phdrRVA);
    m_phnum = elfpeHeader->phdrSize / sizeof(Elf64_Phdr);

    for(auto ptr = m_phdr, limit = m_phdr + m_phnum; ptr < limit; ptr++) {
        const auto &phdr = *ptr;

        if(phdr.p_type == PT_DYNAMIC) {
            m_dynamic = displace<Elf64_Dyn>(phdr.p_vaddr);
        } else if(phdr.p_type == PT_LOAD && (phdr.p_flags & PF_W)) {

            /*
             * The segment need to be marked read-write - Windows marks
             * it read-only because we're doing an IMAGE_RESOURCE mapping.
             */

            DWORD oldProtect;
            if(!VirtualProtect(displace(phdr.p_vaddr), phdr.p_memsz, PAGE_READWRITE, &oldProtect))
                WindowsError::throwLastError();

            if(phdr.p_filesz != 0 && phdr.p_memsz > phdr.p_filesz) {
                /*
                * The tail of the last in-file page needs to be zeroed out, since
                * the way we construct PEs don't promise that.
                */

                auto startFillingAt = phdr.p_vaddr + phdr.p_filesz;
                auto finishFillingAt = std::min<size_t>(phdr.p_vaddr + phdr.p_memsz, (startFillingAt + 4095) & ~4095);

                if(finishFillingAt > startFillingAt) {
                    memset(displace(startFillingAt), 0, finishFillingAt - startFillingAt);
                }
            }
        }
    }
}

ELFExecutableMapperWindows::~ELFExecutableMapperWindows() = default;

ELFExecutableMapperWindows::ImageResource::ImageResource(const std::filesystem::path &filename) {
    m_module = LoadLibraryEx(filename.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if(!m_module)
        WindowsError::throwLastError();
    printf("----- ARM image %ls is loaded at %p\n",
           filename.c_str(), base());

}

ELFExecutableMapperWindows::ImageResource::~ImageResource() {
    FreeLibrary(m_module);
}
