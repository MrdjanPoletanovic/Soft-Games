#define _CRT_SECURE_NO_WARNINGS 
#include "igrac_igranje.h"
#include "loto.h"
#include "broj.h"
#include "poker.h"
#include "kviz.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <random>
#include <chrono>
#include <thread>
#include <sstream>
#include <utility>


constexpr auto naziv_datoteke = "nalozi.csv";
constexpr auto broj_elemenata = 4;


Igrac::Igrac(int line, bool first_time) : nizovi(new KruzniBafer[broj_elemenata]), otkazan(new int[broj_elemenata]), prijavljen(new int[broj_elemenata]), vrijeme_igranja(new double[broj_elemenata])
{
	std::ifstream file(naziv_datoteke);
	if (file.is_open())
	{
		std::string tmp;
		for(int i=0; i<line; i++)
			std::getline(file, tmp);
		std::getline(file, tmp, ',');
		korisnicko_ime = tmp;
		std::getline(file, tmp);
		sifra = tmp;
		if (first_time || std::getline(file, tmp, ',').eof())
		{
			stanje = 10;
			dobitak = gubitak = pokusajLoto = pokusajBroj = 0;
			for(int i=0; i<broj_elemenata; i++)
			{
				prijavljen[i] = otkazan[i] = 0;
				vrijeme_igranja[i] = 0;
			}
		}
		else
		{
			stanje = std::stoi(tmp);
			std::getline(file, tmp, ',');
			gubitak = std::stoi(tmp);
			std::getline(file, tmp, ',');
			dobitak = std::stoi(tmp);
			std::getline(file, tmp, ',');
			pokusajBroj= std::stoi(tmp);
			std::getline(file, tmp);
			pokusajLoto = std::stoi(tmp);
			for(int i=0; i<broj_elemenata; i++)
			{
				if (i != 3)
					std::getline(file, tmp, ',');
				else
					std::getline(file, tmp);
				otkazan[i] = std::stoi(tmp);
			}
			for(int i=0; i<broj_elemenata; i++)
			{
				if (i != 3)
					std::getline(file, tmp, ',');
				else
					std::getline(file, tmp);
				prijavljen[i] = std::stoi(tmp);
			}
			for(int i=0; i<broj_elemenata; i++)
			{
				if (i != 3)
					std::getline(file, tmp, ',');
				else
					std::getline(file, tmp);
				vrijeme_igranja[i] = std::stod(tmp);
			}
			for(int i=0; i<broj_elemenata; i++)
				nizovi[i].readFromFile(file);
			file.close();
		}
		if (!first_time)
		{
			clear_screen();
			std::cout << "Pozdrav, " << korisnicko_ime << "! " << std::endl;
		}
	}
}


Igrac::Igrac(const Igrac& other)
{
	copy(other);
}


Igrac::Igrac(Igrac&& other)
{
	move(std::move(other));
}


Igrac& Igrac::operator=(const Igrac& other)
{
	if (this != &other)
	{
		this->~Igrac();
		copy(other);
	}
	return *this;
}


Igrac& Igrac::operator=(Igrac&& other)
{
	if (this != &other)
	{
		this->~Igrac();
		move(std::move(other));
	}
	return *this;
}


Igrac::~Igrac()
{
	delete [] nizovi;
	delete [] otkazan;
	delete [] prijavljen;
	delete [] vrijeme_igranja;
}


void Igrac::move(Igrac&& other)
{
	korisnicko_ime = std::move(other.korisnicko_ime);
	sifra = std::move(other.sifra);
	stanje = std::move(other.stanje);
	dobitak = std::move(other.dobitak);
	gubitak = std::move(other.gubitak);
	pokusajBroj = std::move(other.pokusajBroj);
	pokusajLoto = std::move(other.pokusajLoto);
	nizovi = other.nizovi;
	prijavljen = other.prijavljen;
	otkazan = other.otkazan;
	vrijeme_igranja = other.vrijeme_igranja;
	other.nizovi = nullptr;
	other.prijavljen = other.otkazan = nullptr;
	other.vrijeme_igranja = nullptr;
}


void Igrac::copy(const Igrac& other)
{
	korisnicko_ime = other.korisnicko_ime;
	sifra = other.sifra;
	stanje = other.stanje;
	gubitak = other.gubitak;
	dobitak = other.dobitak;
	pokusajBroj = other.pokusajBroj;
	pokusajLoto = other.pokusajLoto;
	for (int i = 0; i < broj_elemenata; i++)
	{
		nizovi[i] = other.nizovi[i];
		prijavljen[i] = other.prijavljen[i];
		otkazan[i] = other.otkazan[i];
		vrijeme_igranja[i] = other.vrijeme_igranja[i];
	}
}

std::string convertBodoviToMessageLoto(int x)
{
	std::string poruka;
	switch(x)
	{
	case 0:
	{
		poruka = "Nije pogodjen nijedan broj.";
		break;
	}
	case 10:
	{
		poruka = "Pogodjen 1 broj";
		break;
	}
	case 30:
	{
		poruka = "Pogodjena 2 broja";
		break;
	}
	case 60:
	{
		poruka = "Pogodjena 3 broja";
		break;
	}
	case 100:
	{
		poruka = "Pogodjena 4 broja";
		break;
	}
	case 150:
	{
		poruka = "Pogodjeno 5 brojeva";
		break;
	}
	case 210:
	{
		poruka = "Pogodjeno 6 brojeva";
		break;
	}
	case 280:
	{
		poruka = "Pogodjeno 7 brojeva";
	}
	}
	return poruka;
}


std::string convertBodoviToMessageBroj(int bodovi)
{
	std::string poruka;
	if (bodovi > 0)
	{
		char tmp[100];
		snprintf(tmp, sizeof(tmp), "Pogodjeno iz %d. pokusaja", 100/bodovi);
		poruka = tmp;
	}
	else
	{
		poruka = "Broj nije pogodjen.";
	}
	return poruka;
}


std::string convertBodoviToMessagePoker(int bodovi)
{
	std::string poruka;
	switch(bodovi)
	{
	case 4000:
		poruka = "Royal Flush";
		break;
	case 250:
		poruka = "Straight Flush";
		break;
	case 125:
		poruka = "Poker";
		break;
	case 45:
		poruka = "Full House";
		break;
	case 30:
		poruka = "Flush";
		break;
	case 20:
		poruka = "Straight";
		break;
	case 15:
		poruka = "Three of a Kind";
		break;
	case 10:
		poruka = "Two Pairs";
		break;
	case 5:
		poruka = "Jack's or Better";
		break;
	default:
		poruka = "Nista niste osvojili";
	}
	return poruka;
}


std::string getTime()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
	return oss.str();
}


void Igrac::igraj_loto()
{
	if (otkazan[2] == 0)
	{
		if (stanje >= 100)
		{
			if (prijava(3))
			{
				int bodovi;
				printPravilaLoto();
				time_t start, end = std::time(0);
				do
				{
					start = end;
					if (stanje < 100)
					{
						std::cout << std::endl << "Nemate dovoljno sredstava da igrate loto." << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(1500));
						break;
					}
					clear_screen();
					stanje -= 100;
					gubitak += 100;
					pokusajLoto++;
					writePodaci();
					bodovi = loto(stanje, dobitak, gubitak, pokusajLoto);
					dobitak += bodovi;
					stanje += bodovi;
					nizovi[2].enqueue(bodovi, convertBodoviToMessageLoto(bodovi), getTime());
					end = std::time(0);
					writePodaci();
				} while (provjera_kljuca(3, start, end) && igraj_ponovo());
			}
		}
		else
		{
			std::cout << "Nemate dovoljno sredstava da igrate loto." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		}
	}
	else
	{
		std::cout << "Igra je otkazana. Ne mozete je vise igrati." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
}


void Igrac::igraj_broj()
{
	if (otkazan[0] == 0)
	{
		if (prijava(1))
		{
			int bodovi;
			printPravilaBroj();
			time_t start, end=std::time(0);
			do
			{
				start = end;
				clear_screen();
				bodovi = broj(pokusajBroj);
				pokusajBroj++;
				stanje+=bodovi;
				dobitak+=bodovi;
				nizovi[0].enqueue(bodovi, convertBodoviToMessageBroj(bodovi), getTime());
				end = std::time(0);
				writePodaci();
			}
			while(provjera_kljuca(1, start, end) && igraj_ponovo());
		}
	}
	else
	{
		std::cout << "Igra je otkazana. Ne mozete je vise igrati." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
}


void Igrac::igraj_poker()
{
	if (otkazan[3] == 0)
	{
		if (stanje >= 5)
		{
			if (prijava(4))
			{
				int bodovi;
				printPravilaPoker();
				do
				{
					if (stanje < 5)
					{
						std::cout << std::endl<< "Nemate dovoljno sredstava da igrate poker." << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(1500));
						break;
					}
					clear_screen();
					gubitak += 5;
					stanje -= 5;
					writePodaci();
					bodovi = poker(stanje, dobitak, gubitak);
					nizovi[3].enqueue(bodovi, convertBodoviToMessagePoker(bodovi), getTime());
					dobitak += bodovi;
					stanje += bodovi;
					writePodaci();
				} while (igraj_ponovo());
			}
		}
		else
		{
			std::cout << "Nemate dovoljno sredstava da igrate poker." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		}
	}
	else
	{
		std::cout << "Igra je otkazana. Ne mozete je vise igrati." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
}


std::string convertBodoviToMessageKviz (int tacni, int netacni, int neodgovoreni)
{
	char tmp[100];
        snprintf(tmp, 45, "%d tacnih %d netacnih %d neodgovorenih", tacni, netacni, neodgovoreni);
	std::string poruka = tmp;
	return poruka;
}


void Igrac::igraj_kviz()
{
	if (otkazan[1] == 0)
	{
		if (stanje >= 50)
		{
			if (prijava(2))
			{
				int bodovi, tacni = 0, netacni = 0, neodgovoreni = 0;
				printPravilaKviz();
				time_t start, end = std::time(0);
				do
				{
					start = end;
					if (stanje < 50) // Potrebno znati koliko bodova je potrebno za kviz.
					{
						std::cout << std::endl << "Nemate dovoljno sredstava da igrate kviz." << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(1500));
						break;
					}
					clear_screen();
					stanje -= 50;
					writePodaci();
					bodovi = kviz(stanje, tacni, netacni, neodgovoreni);
					nizovi[1].enqueue(bodovi, convertBodoviToMessageKviz(tacni, netacni, neodgovoreni), getTime());
					dobitak += (tacni < 5) ? (tacni * 20) : tacni * 20 + 50;
					gubitak += netacni * 30 + neodgovoreni * 10;
					tacni = netacni = neodgovoreni = 0;
					end = std::time(0);
					writePodaci();
				} while (provjera_kljuca(2, start, end) && igraj_ponovo());
			}
		}
		else
		{
			std::cout << "Nemate dovoljno sredstava da igrate kviz." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		}
	}
	else
	{
		std::cout << "Igra je otkazana. Ne mozete je vise igrati." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
}


void clear_screen()
{
#ifdef _WIN32
	std::system("cls");
#elif _WIN64
    std::system("cls");
	// Assume POSIX;
#else
	std::system ("clear");
#endif
}


bool Igrac::igraj_ponovo()
{
	int i = 0;
	std::string odgovor;
	do
	{
		std::cout << "Da li zelite ponovo da igrate? (da/ne) ";
		std::getline(std::cin, odgovor);
		for(i=0; odgovor[i]==' '; i++);
		if ((odgovor[i] == 'd' || odgovor[i] == 'D') &&
		    (odgovor[i+1] == 'a' || odgovor[i+1] == 'A'))
			return true;
		else if ((odgovor[i] == 'n' || odgovor[i] == 'N') &&
			 (odgovor[i+1] == 'e' || odgovor[i+1] == 'E'))
			return false;
		std::cout << "Neispravan unos!" << std::endl;
	}
	while(true);
}


bool Igrac::prijava(int redni_broj_igre)
{
	//kontrolna podrazumijeva vrijednost koja se salje iz glavne funkcije i ako ima vrijednost 1 znaci da je igra vec bila aktivirana
	// a ako ima vrijednost 0 znaci da je treba aktivirati
	// str_gener sluzi sa pamcenje generisanog koda u slucaju da se koristi kasnije
	std::string str_gener;
	if (prijavljen[redni_broj_igre-1] == 0)
	{
		std::string str_unos;
		if (redni_broj_igre == 1)
		{
			str_gener = generate("Broj");
			std::cout << "Aktivacioni kljuc: " << str_gener << std::endl;
			std::cout << "Unesite aktivacioni kljuc za igru: ";
		}
		else if (redni_broj_igre == 2)
		{
			str_gener = generate("Kviz");
			std::cout << "Aktivacioni kljuc: " << str_gener << std::endl;
			std::cout << "Unesite aktivacioni kljuc za igru: ";
		}
		else if (redni_broj_igre == 3)
		{
			str_gener = generate("Loto");
			std::cout << "Aktivacioni kljuc: " << str_gener << std::endl;
			std::cout << "Unesite aktivacioni kljuc za igru: ";
		}
		else if (redni_broj_igre == 4)
		{
			str_gener = generate("Poker");
			std::cout << "Aktivacioni kljuc: " << str_gener << std::endl;
			std::cout << "Unesite aktivacioni kljuc za igru: ";
		}
		std::getline(std::cin, str_unos);
		if (str_gener.compare(str_unos) == 0)
		{
			prijavljen[redni_broj_igre-1] = 1;
			return true;
		}
		else
		{
			std::cout << "Aktivacija igre neuspjesna. Pokusajte ponovo." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			return false;
		}
	}
	else
		return true;
}


void Igrac::fill(std::string& str)
{
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distributionInteger(0, 9);
	std::string str1;
	for (int i = 5; i < 19; i++)
	{
		if (i == 9 || i == 14)
			str.insert(i,"-");
		else
		{
			int p = distributionInteger(generator);
			str1 = std::to_string(p);
			str.insert(i, str1);
		}
	}

}


std::string Igrac::generate(std::string s)
{
	std::string key;
	if (s.compare("Loto")==0 )
	{
		key = "3168-";   //broj 3 oznacava redni broj igre, a 168 vrijeme trajanja igre izrazeno u satima
		fill(key);
	}
	else if (s.compare("Broj") == 0)
	{
		key = "1024-";  // broj 1 oznacava redni broj igre, a 024 vrijeme trajanja igre izrazeno u satima
		fill(key);
	}
	else if (s.compare("Kviz") == 0)
	{
		key = "2001-";  // broj 2 oznacava redni broj igre, a 001 vrijeme trajanja igre izrazeno u satima
		fill(key);
	}
	else if (s.compare("Poker") == 0)
	{
		key = "4000-"; //  broj 4 oznacava redni broj igre, a 000 neograniceno trajanje igre
		fill(key);
	}
	return key;
}


void Igrac::otkazi_igru(int redni_broj)
{
	otkazan[redni_broj-1] = 1;
	if (redni_broj == 1)
	{
		std::cout << "--> Vise nemate pristup igri: Pogadjanje broja!\n";
	}
	else if (redni_broj == 2)
	{
		std::cout << "--> Vise nemate pristup igri: Kviz!\n";
	}
	else if (redni_broj == 3)
	{
		std::cout << "--> Vise nemate pristup igri: Loto!\n";
	}
	else if (redni_broj == 4)
	{
		std::cout << "--> Vise nemate pristup igri: Video Poker!\n";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void Igrac::otkazi()
{
	std::cout << "Koju igru zelite otkazati?\n" << "1. Pogadjanje broja\n" << "2. Kviz\n" << "3. Loto\n" << "4. Video Poker\n" << "5. Odustani\n";
	std::cout << "\nUnesite indeks opcije koju zelite izabrati: ";
	std::string odgovor;
	std::getline(std::cin, odgovor);
	
	if (!odgovor.compare("1") || !odgovor.compare("2") || !odgovor.compare("3") || !odgovor.compare("4"))
	{
		int a = std::stoi(odgovor);
		otkazi_igru(a);
	}
	else
		std::cout << "--> Nijedna igra nije otkazana!\n";

}

bool Igrac::provjera_kljuca(int redni_broj, time_t begin, time_t end)
{
	double vrijeme = 0;
	int index = redni_broj-1;
	if (redni_broj == 1)
	{
		vrijeme = 86'400;
		vrijeme_igranja[index] += difftime(end, begin);
		if (vrijeme_igranja[index] >= vrijeme)
		{
			otkazi_igru(redni_broj);
			return false;
		}
		else
			return true;
	}
	else if (redni_broj == 2)
	{
		vrijeme = 3'600;
		vrijeme_igranja[index] += difftime(end, begin);
		if (vrijeme_igranja[index] >= vrijeme)
		{
			otkazi_igru(redni_broj);
			return false;
		}
		else
			return true;
	}
	else if (redni_broj == 3)
	{
		vrijeme = 604'800;
		vrijeme_igranja[index] += difftime(end, begin);
		if (vrijeme_igranja[index] >= vrijeme)
		{
			otkazi_igru(redni_broj);
			return false;
		}
		else
			return true;
	}
	else if (redni_broj == 4)
	{
		return true;
	}
	else
		return false;
}


int Igrac::findName() const
{
	std::ifstream file(naziv_datoteke);
	if (file.is_open())
	{
		int line = 0;
		std::string tmp;
		while(std::getline(file, tmp, ','))
		{
			if (tmp == korisnicko_ime)
				return line;
			std::getline(file, tmp);
			for(int i=0; i<44; i++)
				std::getline(file, tmp);
			line += 45;
		}
	}
	return -1;
}


void Igrac::writePodaci() const
{
	write(findName());
}


void Igrac::write(int line) const
{
	std::ifstream file;
	std::ofstream new_file("new_file.csv");
	std::string content;
	if (line == -1)
		file.open(naziv_datoteke, std::ios::app);
	else
	{
		file.open(naziv_datoteke);
		for(int i=0; i<line; i++)
		{
			std::getline(file, content);
			new_file << content << "\n";
		}
	}
	new_file << korisnicko_ime << "," << sifra << "\n";
	new_file << stanje << "," << gubitak << "," << dobitak << "," << pokusajBroj << "," << pokusajLoto << "\n";
	for(int i=0; i<broj_elemenata; i++)
	{
		if (i != 3)
			new_file << otkazan[i] << ",";
		else
			new_file << otkazan[i] << "\n";
	}
	for(int i=0; i<broj_elemenata; i++)
	{
		if (i != 3)
			new_file << prijavljen[i] << ",";
		else
			new_file << prijavljen[i] << "\n";
	}
	for(int i=0; i<broj_elemenata; i++)
	{
		if (i != 3)
			new_file << vrijeme_igranja[i] << ",";
		else
			new_file << vrijeme_igranja[i] << "\n";
	}
	for(int i=0; i<broj_elemenata; i++)
		new_file << nizovi[i];
	for(int i=0; i<45; i++)
		std::getline(file, content);
	while(std::getline(file, content))
		new_file << content << "\n";
	file.close();
	new_file.close();
	std::remove("nalozi.csv");
	std::rename("new_file.csv", "nalozi.csv");
}


void Igrac::printAllStat() const
{
	std::cout << "IGRA POGADJANJE BROJA: " << std::endl << std::endl;
	nizovi[0].print();
	std::cout << std::endl;
	std::cout << "IGRA KVIZ: : " << std::endl << std::endl;
	nizovi[1].print();
	std::cout << std::endl;
	std::cout << "IGRA LOTO: " << std::endl << std::endl;
	nizovi[2].print();
	std::cout << std::endl;
	std::cout << "IGRA POKER: " << std::endl << std::endl;
	nizovi[3].print();
	std::cout << std::endl;
}
