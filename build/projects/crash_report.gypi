{
  'conditions': [
    ['target_arch == "linux"', {
      'targets': [
        {
          'target_name': 'crash_report',
          'type': 'executable',
          'toolsets': ['host'],
          'include_dirs': [
            '../../../',
            '<(DEPTH)/',
            '<(DEPTH)/breakpad/',
            '<(DEPTH)/breakpad/src/',
            '<(DEPTH)/breakpad/src/client/',
            '<(DEPTH)/third_party/linux/include',
          ],
          'sources': [
            '../../crash_report/core2md.cc',
            '../../crash_report/core_dumper.cc',
            '../../crash_report/minidump_writer.cc',
            '../../crash_report/symbol_info.cc',
            '../../crash_report/common/elf_core_dump.cc',
            '<(DEPTH)/breakpad/src/client/linux/handler/minidump_descriptor.h',
            '<(DEPTH)/breakpad/src/client/linux/handler/minidump_descriptor.cc',
            '<(DEPTH)/breakpad/src/client/minidump_file_writer-inl.h',
            '<(DEPTH)/breakpad/src/client/minidump_file_writer.h',
            '<(DEPTH)/breakpad/src/client/minidump_file_writer.cc',
            '<(DEPTH)/breakpad/src/common/string_conversion.cc',
            '<(DEPTH)/breakpad/src/common/string_conversion.h',
            '<(DEPTH)/breakpad/src/common/convert_UTF.c',
            '<(DEPTH)/breakpad/src/common/convert_UTF.h',
            '<(DEPTH)/breakpad/src/common/memory.h',
            '<(DEPTH)/breakpad/src/common/memory_range.h',
            '<(DEPTH)/breakpad/src/common/scoped_ptr.h',
            '<(DEPTH)/breakpad/src/common/linux/guid_creator.cc',
            '<(DEPTH)/breakpad/src/common/linux/guid_creator.h',
            '<(DEPTH)/breakpad/src/common/linux/linux_libc_support.cc',
            '<(DEPTH)/breakpad/src/common/linux/linux_libc_support.h',
            '<(DEPTH)/breakpad/src/common/linux/memory_mapped_file.cc',
            '<(DEPTH)/breakpad/src/common/linux/memory_mapped_file.h',
            '<!@(find <(DEPTH)/breakpad/src/google_breakpad/common/ -type f)',
          ],
          'libraries_host': [
            '<@(platform_libraries)',
          ],
          'link_settings': {
            'libraries_host': [
              '-ldl',
            ],
          },
        },
        {
          'target_name': 'minidump_uploader',
          'type': 'executable',
          'toolsets': ['host'],
          'sources': [
            '<(DEPTH)/breakpad/src/tools/linux/symupload/minidump_upload.cc',
            '<(DEPTH)/breakpad/src/common/linux/http_upload.h',
            '<(DEPTH)/breakpad/src/common/linux/http_upload.cc',
            '<(DEPTH)/breakpad/src/common/using_std_string.h',
            '<!find <(DEPTH)/breakpad/src/third_party/curl/ -type f)',
          ],
          'include_dirs': [
            '../../../',
            '<(DEPTH)/breakpad/src',
            '<(DEPTH)/breakpad/src/third_party',
          ],
          'link_settings': {
            'libraries_host': [
              '-ldl',
            ],
          },
        },
        {
          'target_name': 'symupload',
          'type': 'executable',
          'toolsets': ['host'],
          'sources': [
            '<(DEPTH)/breakpad/src/tools/linux/symupload/sym_upload.cc',
            '<(DEPTH)/breakpad/src/common/linux/http_upload.cc',
            '<(DEPTH)/breakpad/src/common/linux/http_upload.h',
          ],
          'include_dirs': [
            '../../../',
            '<(DEPTH)/breakpad/src',
            '<(DEPTH)/breakpad/src/third_party',
          ],
          'link_settings': {
            'libraries_host': [
              '-ldl',
            ],
          },
        },
        {
          'target_name': 'minidump_stackwalk',
          'type': 'executable',
          'toolsets': ['host'],
          'sources': [
            '<(DEPTH)/breakpad/src/processor/basic_code_module.h',
            '<(DEPTH)/breakpad/src/processor/basic_code_modules.cc',
            '<(DEPTH)/breakpad/src/processor/basic_code_modules.h',
            '<(DEPTH)/breakpad/src/processor/basic_source_line_resolver.cc',
            '<(DEPTH)/breakpad/src/processor/binarystream.cc',
            '<(DEPTH)/breakpad/src/processor/binarystream.h',
            '<(DEPTH)/breakpad/src/processor/call_stack.cc',
            '<(DEPTH)/breakpad/src/processor/cfi_frame_info.cc',
            '<(DEPTH)/breakpad/src/processor/cfi_frame_info.h',
            '<(DEPTH)/breakpad/src/processor/disassembler_x86.cc',
            '<(DEPTH)/breakpad/src/processor/disassembler_x86.h',
            '<(DEPTH)/breakpad/src/processor/exploitability.cc',
            '<(DEPTH)/breakpad/src/processor/exploitability_win.cc',
            '<(DEPTH)/breakpad/src/processor/exploitability_win.h',
            '<(DEPTH)/breakpad/src/processor/logging.cc',
            '<(DEPTH)/breakpad/src/processor/logging.h',
            '<(DEPTH)/breakpad/src/processor/minidump.cc',
            '<(DEPTH)/breakpad/src/processor/minidump_processor.cc',
            '<(DEPTH)/breakpad/src/processor/minidump_stackwalk.cc',
            '<(DEPTH)/breakpad/src/processor/pathname_stripper.cc',
            '<(DEPTH)/breakpad/src/processor/pathname_stripper.h',
            '<(DEPTH)/breakpad/src/processor/process_state.cc',
            '<(DEPTH)/breakpad/src/processor/simple_symbol_supplier.cc',
            '<(DEPTH)/breakpad/src/processor/simple_symbol_supplier.h',
            '<(DEPTH)/breakpad/src/processor/source_line_resolver_base.cc',
            '<(DEPTH)/breakpad/src/processor/stack_frame_symbolizer.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_amd64.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_amd64.h',
            '<(DEPTH)/breakpad/src/processor/stackwalker_arm.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_arm.h',
            '<(DEPTH)/breakpad/src/processor/stackwalker_ppc.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_ppc.h',
            '<(DEPTH)/breakpad/src/processor/stackwalker_ppc64.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_ppc64.h',
            '<(DEPTH)/breakpad/src/processor/stackwalker_sparc.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_sparc.h',
            '<(DEPTH)/breakpad/src/processor/stackwalker_x86.cc',
            '<(DEPTH)/breakpad/src/processor/stackwalker_x86.h',
            '<(DEPTH)/breakpad/src/processor/tokenize.cc',
            '<(DEPTH)/breakpad/src/processor/tokenize.h',
            # libdisasm
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_implicit.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_implicit.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_insn.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_insn.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_invariant.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_invariant.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_modrm.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_modrm.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_opcode_tables.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_opcode_tables.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_operand.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_operand.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_reg.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_reg.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_settings.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/ia32_settings.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/libdis.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/qword.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_disasm.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_format.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_imm.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_imm.h',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_insn.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_misc.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_operand_list.c',
            '<(DEPTH)/breakpad/src/third_party/libdisasm/x86_operand_list.h',
          ],
          'include_dirs': [
            '../../../',
            '<(DEPTH)/breakpad/src',
            '<(DEPTH)/breakpad/src/processor',
            '<(DEPTH)/breakpad/src/third_party',
          ],
          'variables': {
            'host_arch': '<!(uname -m)',
          },
        },
      ],
    }],
  ],
}
