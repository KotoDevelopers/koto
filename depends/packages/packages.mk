rust_crates := \
  crate_addchain \
  crate_aes \
  crate_aesni \
  crate_aes_soft \
  crate_ansi_term \
  crate_arrayref \
  crate_arrayvec \
  crate_autocfg \
  crate_base64 \
  crate_bellman \
  crate_bigint \
  crate_bit_vec \
  crate_blake2b_simd \
  crate_blake2s_simd \
  crate_block_buffer \
  crate_block_cipher \
  crate_block_modes \
  crate_block_padding \
  crate_bls12_381 \
  crate_byteorder \
  crate_cfg_if \
  crate_chrono \
  crate_constant_time_eq \
  crate_cpuid_bool \
  crate_crossbeam_channel \
  crate_crossbeam_deque \
  crate_crossbeam_epoch \
  crate_crossbeam_queue \
  crate_crossbeam_utils \
  crate_crossbeam \
  crate_crunchy \
  crate_crypto_api_chachapoly \
  crate_crypto_api \
  crate_curve25519_dalek \
  crate_digest \
  crate_directories \
  crate_dirs_sys \
  crate_ed25519_zebra \
  crate_equihash \
  crate_ff_derive \
  crate_ff \
  crate_fpe \
  crate_futures_cpupool \
  crate_futures \
  crate_generic_array \
  crate_getrandom \
  crate_group \
  crate_hermit_abi \
  crate_hex \
  crate_jubjub \
  crate_lazy_static \
  crate_libc \
  crate_log \
  crate_matchers \
  crate_maybe_uninit \
  crate_memoffset \
  crate_num_bigint \
  crate_num_cpus \
  crate_num_integer \
  crate_num_traits \
  crate_opaque_debug \
  crate_pairing \
  crate_ppv_lite86 \
  crate_proc_macro2 \
  crate_quote \
  crate_rand_chacha \
  crate_rand_core \
  crate_rand_hc \
  crate_rand_xorshift \
  crate_rand \
  crate_redox_syscall \
  crate_redox_users \
  crate_regex_automata \
  crate_regex_syntax \
  crate_regex \
  crate_rust_argon2 \
  crate_scopeguard \
  crate_serde \
  crate_serde_derive \
  crate_sha2 \
  crate_sharded_slab \
  crate_subtle \
  crate_syn \
  crate_time \
  crate_thiserror \
  crate_thiserror_impl \
  crate_thread_local \
  crate_tracing_appender \
  crate_tracing_attributes \
  crate_tracing_core \
  crate_tracing_subscriber \
  crate_tracing \
  crate_typenum \
  crate_unicode_xid \
  crate_version_check \
  crate_wasi \
  crate_winapi_i686_pc_windows_gnu \
  crate_winapi \
  crate_winapi_x86_64_pc_windows_gnu \
  crate_zcash_history \
  crate_zcash_primitives \
  crate_zcash_proofs \
  crate_zeroize
zcash_packages := libsodium utfcpp
packages := boost openssl libevent zeromq $(zcash_packages) googletest curl
native_packages := native_ccache native_rust

qt_native_packages = native_protobuf
qt_packages = qrencode protobuf zlib

qt_linux_packages:=qt expat dbus libxcb xcb_proto libXau xproto freetype fontconfig libX11 xextproto libXext xtrans

qt_darwin_packages=qt
qt_mingw32_packages=qt

wallet_packages=bdb

darwin_native_packages = native_biplist native_ds_store native_mac_alias

ifneq ($(build_os),darwin)
darwin_native_packages += native_cctools native_cdrkit native_libdmg-hfsplus
endif
