option(VANADIUM_USE_ASAN "Enable AddressSanitizer" OFF)
option(VANADIUM_USE_TSAN "Enable ThreadSanitizer" OFF)
option(VANADIUM_USE_MSAN "Enable MemorySanitizer" OFF)
option(VANADIUM_USE_UBSAN "Enable UndefinedBehaviourSanitizer" OFF)

function(sanitize name)
  add_compile_options("-fsanitize=${name}")
  add_link_options("-fsanitize=${name}")
endfunction()

if(VANADIUM_USE_ASAN)
  sanitize(address)
endif()
#
if(VANADIUM_USE_TSAN)
  sanitize(thread)
endif()
#
if(VANADIUM_USE_MSAN)
  sanitize(memory)
endif()
#
if(VANADIUM_USE_UBSAN)
  sanitize(undefined)
endif()
