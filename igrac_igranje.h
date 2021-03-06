#pragma once
#include <string>
#include "statistika.h"


class Igrac
{
	std::string korisnicko_ime;
	std::string sifra;
	int dobitak, gubitak, stanje;
	int pokusajBroj, pokusajLoto;
	KruzniBafer *nizovi;
	int *otkazan, *prijavljen;
	double *vrijeme_igranja;
public:
	Igrac(int, bool);
	Igrac(const Igrac&);
	Igrac(Igrac&&);
	Igrac& operator=(const Igrac&);
	Igrac& operator=(Igrac&&);
	//inline int getDobitak() const {return dobitak;}
	//inline int getGubitak() const{return gubitak;}
	inline int getStanje() const {return stanje;}
	inline void printStatBroj() const {nizovi[0].print();}
	inline void printStatKviz() const {nizovi[1].print();}
	inline void printStatLoto() const {nizovi[2].print();}
	inline void printStatPoker() const {nizovi[3].print();}
	void printAllStat() const;
	void writePodaci() const;
	void otkazi_igru(int);
	void otkazi();
	void igraj_loto();
	void igraj_poker();
	void igraj_broj();
	void igraj_kviz();
	~Igrac();
private:
	void move(Igrac&&);
	void copy(const Igrac&);
	bool prijava(int);
	void fill(std::string&);
	std::string generate(std::string);
	bool provjera_kljuca(int, time_t, time_t);
	bool igraj_ponovo();
	void write(int) const;
	int findName() const;
};


void clear_screen();

