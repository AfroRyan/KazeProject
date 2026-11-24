CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -I../src -I../../_shared
SRC = ../src/physics.c
SMOKE = smoke_main.c

WASM_OUT = ../../compilation/assets/physics/lg_phys.wasm
WASM_CC ?= clang
WASM_FLAGS = --target=wasm32 -nostdlib -O2 \
	-Wl,--no-entry \
	-Wl,--export=phys_mask_contact_flags_from_cmd \
	-Wl,--export=phys_mask_bounce_type \
	-Wl,--export=phys_mask_load_be \
	-Wl,--export=phys_mask_poly_parse \
	-Wl,--export=phys_wasm_bind_mask \
	-Wl,--export=phys_wasm_bind_poly \
	-Wl,--export=phys_wasm_nsegs \
	-Wl,--export=phys_wasm_poly_ncontours \
	-Wl,--export=phys_wasm_poly_nverts \
	-Wl,--export=phys_wasm_poly_npalette \
	-Wl,--export=phys_wasm_poly_bytes \
	-Wl,--export=phys_wasm_seg_contact_flags \
	-Wl,--export=phys_wasm_seg_bounce_type \
	-Wl,--export=phys_world_init_shell \
	-Wl,--export=phys_live_tick_frame \
	-I../src -Iwasm/include

.PHONY: all run clean wasm

all: smoke

smoke: $(SMOKE) $(SRC) ../src/physics.h
	$(CC) $(CFLAGS) -o smoke $(SMOKE) $(SRC)

run: smoke
	./smoke

wasm: $(SRC) ../src/physics.h wasm/wasm_shims.c wasm/include/string.h
	$(WASM_CC) $(WASM_FLAGS) -o $(WASM_OUT) $(SRC) wasm/wasm_shims.c

clean:
	rm -f smoke
