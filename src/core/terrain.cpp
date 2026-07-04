#include "terrain.hpp"
#include "sector.hpp"
#include "kore.hpp"

static int sum_table[20 * 21] = { // For animations
	18,48,48,48,48,18,18,18,18,48,48,48,48,18,48,18,18,48,48,18,
	48,19,48,48,19,48,19,19,48,19,48,48,19,48,48,19,48,19,48,19,
	48,48,20,48,20,20,48,20,48,20,48,20,48,48,48,20,20,48,20,48,
	48,48,48,21,21,21,21,48,21,48,21,48,48,48,48,21,48,21,21,48,
	48,19,20,21,22,36,35,27,21,27,28,29,30,32,32,22,20,35,36,19,
	18,48,20,21,36,23,26,34,26,29,28,29,32,31,32,23,34,21,36,18,
	18,19,48,21,35,26,24,37,26,30,28,32,30,31,32,24,18,35,21,37,
	18,19,20,48,27,34,37,25,18,27,32,29,30,31,32,25,34,19,20,37,
	18,48,48,21,21,26,26,18,26,32,28,32,32,31,32,26,18,21,21,18,
	48,19,20,48,27,29,30,27,32,27,32,29,30,32,32,27,20,19,20,19,
	48,48,48,21,28,28,28,32,28,32,28,32,32,32,32,21,48,21,21,48,
	48,48,20,48,29,29,32,29,32,29,32,29,32,32,32,20,20,48,20,48,
	48,19,48,48,30,32,30,30,32,30,32,32,30,32,32,19,48,19,19,19,
	18,48,48,48,32,31,31,31,31,32,32,32,32,31,32,18,18,48,18,18,
	48,48,48,48,32,32,32,32,32,32,32,32,32,32,32,48,48,48,48,48,
	18,19,20,21,22,23,24,25,26,27,21,20,19,18,48,33,34,35,36,37,
	18,48,20,48,20,34,18,34,18,20,21,20,48,18,48,34,34,48,20,18,
	48,19,48,21,35,21,35,19,21,19,21,48,19,18,48,35,48,35,21,19,
	48,48,20,21,36,36,21,20,21,20,21,20,48,48,48,36,20,21,36,48,
	18,19,48,48,19,18,37,37,18,19,48,48,19,18,48,37,18,19,48,37,
	 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 1, 2, 3,
};

static const int additive_table[16 * 16] = { // For static terrain
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	1,1,16,1,12,13,1,12,13,13,12,15,12,13,15,15,
	2,16,2,11,2,2,14,11,8,11,14,11,15,15,14,15,
	3,1,11,3,7,9,1,7,13,9,12,11,12,13,15,15,
	4,12,2,7,4,2,10,7,14,11,10,11,12,15,15,15,
	5,13,2,9,2,5,8,11,8,9,14,11,15,13,14,15,
	6,1,14,1,10,8,6,12,8,10,10,15,12,13,14,15,
	7,12,11,7,7,11,12,7,15,11,12,11,12,15,15,15,
	8,13,14,13,14,8,8,15,8,13,14,15,15,13,14,15,
	9,13,11,9,11,9,10,11,13,9,15,11,15,13,15,15,
	10,12,14,12,10,14,10,12,14,15,10,15,12,15,14,15,
	11,15,11,11,11,11,15,11,15,11,15,11,15,15,15,15,
	12,12,12,12,12,15,12,12,15,15,12,15,12,15,15,15,
	13,13,13,13,15,13,13,15,13,13,15,15,15,13,15,15,
	14,15,14,15,15,14,14,15,14,15,14,15,15,15,14,15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};

void Terrain::draw(Layer *layer, int index, int tx, int ty) {
	//if ((ty<0 || ty >= layer->height)
	//|| (tx <0 || tx >= layer->width)) 
	//	return;
	if (index <= -1)index = this->base;
	if (index == 48)index = this->base;
	else if (index >= 18 && index <= 47) {
		int dest = layer->get(tx, ty);
		if (dest >= 18 && dest <= 47) {
			int x = index-18;
			int y = dest-18;//18=sand tile start
			index = sum_table[(y * 20) + x];
		}
		else if (dest== this->base) index = this->base;
	}
	if (index == 48)index = this->base;
	layer->set(tx, ty, index);
	return;
}

void Terrain::randomize(Layer *layer,int material, int density) {
	for (int c = 0; c < density; c++) {
		int tx = TRUERAND(Sector::size);
		int ty = TRUERAND(Sector::size);
		int already = layer->get(tx, ty);
		if (already < 0)
			continue;
		if (already >= this->base) {
			int y = (already - this->base) % 16;
			int x = 3+(TRUERAND(4));
			layer->set(tx, ty, additive_table[(y * 16) + x]+(material*16)+this->base);
		}
	}
}

void Terrain::fixBorders(Layer *layer) {
	Sector *p = (Sector *)layer->parent;
	// ----------------------------------------
	// JOIN GAPS BETWEEN NEIGHBORS
	// ----------------------------------------
	if (p->sibiling[Direction::NORTH]) {
		for(int x = 0; x < Sector::size; x++) {
			int tx = x;
			int ty = -1;
			if (layer->get(tx, ty)) {
				this->drawSquare(layer, this->size_s, tx, ty);
				if((TRUERAND(2))==1)x++;
			}
		}		
	}
	if (p->sibiling[Direction::SOUTH]) {
		for(int x = 0; x < Sector::size; x++) {
			int tx = x;
			int ty = Sector::size-1;
			if (layer->get(tx, ty)) {
				this->drawSquare(layer, this->size_s, tx, ty);
				if((TRUERAND(2))==1)x++;
			}

		}
	}
	if (p->sibiling[Direction::LEFT]) {
		for(int y = 0; y < Sector::size; y++) {
			int tx = -1;
			int ty = y;
			if (layer->get(tx, ty)) {
				this->drawSquare(layer, this->size_s, tx, ty);
				if((TRUERAND(2))==1)y++;
			}
		}
	}
	if (p->sibiling[Direction::RIGHT]) {
		for(int y = 0; y < Sector::size; y++) {
			int tx = Sector::size-1;
			int ty = y;
			if (layer->get(tx, ty)) {
				this->drawSquare(layer, this->size_s, tx, ty);
				if((TRUERAND(2))==1)y++;
			}
		}
	}
	// ----------------------------------------
	// FIX GAPS
	// ----------------------------------------
	// Replace [ ] + [|] by [ ] [ ]
	// Replace [ ] + [-] by [ ] [ ]
	// ----------------------------------------
	for (int y = 0; y < Sector::size; y++) {
		for (int x = 0; x < Sector::size; x++) {
			if (layer->get(x, y) == 37) {
				if (layer->get(x, y - 1) >= this->base) {
					layer->set(x, y, this->base);
				}
			}
			if (layer->get(x, y) == 34) {
				if (layer->get(x-1, y) >= this->base) {
					layer->set(x, y, this->base);
				}
			}
		}
	}
	
}

void Terrain::fill(Layer *layer,int material, int &_probability) {
	float probability = _probability*2;
	for (int tx = 0; tx < Sector::size; tx++) {
		for (int ty = 0; ty < Sector::size; ty++) {
			int already = layer->get(tx, ty);
			if (already < 0)
				continue;
			if (float(TRUERAND(100)) >= probability)continue;
			probability += ((float(rand()) / float(RAND_MAX)) * 0.0010f ) - 0.0005f;
			if (already >= this->base+((material-2)*16)) {
				int tnw = layer->get(tx    , ty    );
				int tne = layer->get(tx + 1, ty    );
				int tsw = layer->get(tx    , ty + 1);
				int tse = layer->get(tx + 1, ty + 1);
				int ynw = (tnw - this->base) % 16;
				int yne = (tne - this->base) % 16;
				int ysw = (tsw - this->base) % 16;
				int yse = (tse - this->base) % 16;
				int nw = 3;
				int ne = 4;
				int sw = 5;
				int se = 6;
				if ((tnw >= base) && (tne >= base) && (tsw >= base) && (tse >= base)){
					layer->set(tx		, ty	, additive_table[(ynw * 16) + nw] + (material * 16) + this->base);
					layer->set(tx + 1	, ty	, additive_table[(yne * 16) + ne] + (material * 16) + this->base);
					layer->set(tx		, ty + 1, additive_table[(ysw * 16) + sw] + (material * 16) + this->base);
					layer->set(tx + 1	, ty + 1, additive_table[(yse * 16) + se] + (material * 16) + this->base);
				}
			}
		}
	}
	_probability = probability / 2;
}
void Terrain::fillSand(Layer *layer, int material) {
	for (int tx = 0; tx < Sector::size; tx++) {
		for (int ty = 0; ty < Sector::size; ty++) {
			int already = layer->get(tx, ty);
			if (already < 0)
				continue;
			int begin = 64;
			int end   = 64+16;
			if (already >= begin && already < end) {
				int tnw = layer->get(tx    , ty    );
				int tne = layer->get(tx + 1, ty    );
				int tsw = layer->get(tx    , ty + 1);
				int tse = layer->get(tx + 1, ty + 1);
				int ynw = tnw < base ? 0 : (tnw - base )%16;
				int yse = tse < base ? 0 : (tse - base )%16;
				int yne = tne < base ? 0 : (tne - base )%16;
				int ysw = tsw < base ? 0 : (tsw - base )%16;
				int nw = 3;
				int ne = 4;
				int sw = 5;
				int se = 6;
				if((tnw >= begin) && (tnw < end)
				&& (tne >= begin) && (tne < end)
				&& (tsw >= begin) && (tsw < end)
				&& (tse >= begin) && (tse < end)
				){
					int b = begin;
					layer->set(tx		, ty	, additive_table[(ynw * 16) + nw] + b);
					layer->set(tx + 1	, ty	, additive_table[(yne * 16) + ne] + b);
					layer->set(tx		, ty + 1, additive_table[(ysw * 16) + sw] + b);
					layer->set(tx + 1	, ty + 1, additive_table[(yse * 16) + se] + b);					
				}
			}
		}
	}
}

void Terrain::fillRestricted(Layer *layer,int material, int offset, Layer *destination) {
	if (destination == nullptr)destination = layer;
	for (int tx = 0; tx < Sector::size; tx++) {
		for (int ty = 0; ty < Sector::size; ty++) {
			int already = layer->get(tx, ty);
			if (already < 0)
				continue;
			int begin = offset + ((material - 3) * 16);
			int end   = offset + ((material - 2) * 16);
			if (already >= begin && already < end) {
				int tnw = layer->get(tx    , ty    );
				int tne = layer->get(tx + 1, ty    );
				int tsw = layer->get(tx    , ty + 1);
				int tse = layer->get(tx + 1, ty + 1);
				int ynw = tnw < (offset+1) ? 0 : (tnw - (offset+1) )%16;
				int yne = tne < (offset+1) ? 0 : (tne - (offset+1) )%16;
				int ysw = tsw < (offset+1) ? 0 : (tsw - (offset+1) )%16;
				int yse = tse < (offset+1) ? 0 : (tse - (offset+1) )%16;
				int nw = 3;
				int ne = 4;
				int sw = 5;
				int se = 6;
				if((tnw >= begin) 
				&& (tne >= begin) 
				&& (tsw >= begin) 
				&& (tse >= begin)
				){
					int b = ((material - 3) * 16) + (offset+1);
					layer->set(tx		, ty	, additive_table[(ynw * 16) + nw] + b);
					layer->set(tx + 1	, ty	, additive_table[(yne * 16) + ne] + b);
					layer->set(tx		, ty + 1, additive_table[(ysw * 16) + sw] + b);
					layer->set(tx + 1	, ty + 1, additive_table[(yse * 16) + se] + b);
					if (destination != layer) {
						b += 16;
						destination->set(tx		, ty	, additive_table[(ynw * 16) + nw] + b);
						destination->set(tx + 1	, ty	, additive_table[(yne * 16) + ne] + b);
						destination->set(tx		, ty + 1, additive_table[(ysw * 16) + sw] + b);
						destination->set(tx + 1	, ty + 1, additive_table[(yse * 16) + se] + b);						
						b -= 16;
					}
				}
			}
		}
	}
}

void Terrain::drawSquare(Layer *layer, int *data, int x, int y){	
	int tx = x;
	int ty = y;
	this->draw(layer, data[0],   tx,   ty);
	this->draw(layer, data[1], tx+1,   ty);
	this->draw(layer, data[2],   tx, ty+1);
	this->draw(layer, data[3], tx+1, ty+1);
}

void Terrain::createIsland(Layer *layer, int x, int y, int size) {
	int tx = x;
	int ty = y;
	switch (size) {
		case 0:
			this->draw(layer, this->size_xs, tx, ty);
			return;
		case 1:
			this->drawSquare(layer, this->size_s,   tx,   ty);
			return;
		case 2:
			this->draw(layer, this->size_m[0],   tx,   ty);
			this->draw(layer, this->size_m[1], tx+1,   ty);
			this->draw(layer, this->size_m[2], tx+2,   ty);
			this->draw(layer, this->size_m[3],   tx, ty+1);
			this->draw(layer, this->size_m[4], tx+1, ty+1);
			this->draw(layer, this->size_m[5], tx+2, ty+1);
			this->draw(layer, this->size_m[6],   tx, ty+2);
			this->draw(layer, this->size_m[7], tx+1, ty+2);
			this->draw(layer, this->size_m[8], tx+2, ty+2);
			return;
		default:
			size += 2;
			for (int ly = ty + 1; ly < ty + size - 1; ly++) {
				for (int lx = tx + 1; lx < tx + size - 1; lx++) {
					this->draw(layer, this->base, lx, ly);
				}
			}
			for(int ly=ty+2; ly<ty+size-2; ly++){
				this->draw(layer, this->size_loop_w, tx			, ly);
				this->draw(layer, this->size_loop_e, tx+size-1	, ly);
				
			}
			for (int lx=tx+2; lx<tx+size-2; lx++) {
				this->draw(layer, this->size_loop_n, lx			, ty);
				this->draw(layer, this->size_loop_s, lx			, ty+size-1);
			}
			this->drawSquare(layer, this->size_loop_nw,   tx,   ty);
			this->drawSquare(layer, this->size_loop_ne, tx+size-2,   ty);
			this->drawSquare(layer, this->size_loop_sw,   tx, ty+size-2);
			this->drawSquare(layer, this->size_loop_se, tx+size-2, ty+size-2);
			return;
	}
}
