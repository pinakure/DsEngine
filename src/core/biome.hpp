#ifndef CORE_BIOME
#define CORE_BIOME

typedef enum E_BiomeType {
	BIOME_DESSERT,
	BIOME_PLAINS,
	BIOME_ISLANDS,
	BIOME_OCEAN,
	BIOME_WOODS
}BiomeType;

class Biome {
private:
public:
	int moisture;
	int density;
	int temperature;
	int vegetation;
	BiomeType type;

	Biome(int moisture, int density, int temperature, BiomeType type	);
	static Biome *create(BiomeType type);
};

#endif