// Copyright 2019 Isar Ioana Teodora 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define NMAX 50
#define NMAX2 100
#define NMAX3 15
#define NMAX4 512
union record {
  char charptr[NMAX4];
  struct header {
    char name[NMAX2];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[NMAX2];
    char magic[8];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
  } header;
};

// se converteste un numar in octal si se memoreaza intr-un vector
// se returneaza lungimea numarului
int conversion_octal(int number, int octal[NMAX2]) {
    int i = 0;

    while (number != 0) {
        octal[i] = number % 8;
        number = number / 8;
        i++;
    }

    return i;
}

// se returneaza un numar din octal in zecimal
int conversion_decimal(int octal) {
    int number, power;
    power = 1;
    number = 0;

    while (octal != 0) {
        number = number + (octal % 10) * power;
        power = power * 8;
        octal = octal / 10;
    }

    return number;
}

// se transforma un numar in octal si se retine la finalul unui vector char
void conversion(char number[NMAX], char octal[NMAX2], int dimension) {
    int i, j, n, dim, nr, number2[NMAX];
    j = 0;

    for (i = 0; i < 50; i++) {
        number2[i] = 0;
    }

    n = strlen(number);

// se transforma numarul memorat in vectorul char intr-un vector int
    for (i = 0; i < n; i++) {
        number2[i] = number[i] - '0';
    }

    nr = 0;

// se retine numarul din vector intr-o variabila
    for (i = 0; i < n; i++) {
        nr = nr * 10 + number2[i];
    }

// se transforma numarul in octal si se retine lungimea sa
    dim = conversion_octal(nr, number2);
    memset(octal, '0', dimension);
    octal[dimension - 1] = '\0';

// se retine numarul in octal la finalul unui vector char
    for (i = dimension - 2; i >= dimension - 1 - dim; i--) {
        octal[i] = number2[j] + '0';
        j++;
    }
}

// se calculeaza campul mtime din header folosind data crearii fisierului
void timp(char date1[NMAX3], char date2[30], char mmtime[12]) {
    char date3[30], *value;
    int i, j, time, dim, n, dimension, number2[NMAX2];
    struct tm date;

    for (i = 0; i < 100; i++) {
        number2[i] = 0;
    }

// se calculeaza valorile ce trebuie introduse in structura date
    value = strtok(date1, "-");
    strcpy(date3, value);
    date.tm_year = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_year = date.tm_year * 10 + (date3[i] - '0');
    }

    date.tm_year = date.tm_year - 1900;
    value = strtok(NULL, "-");
    strcpy(date3, value);
    date.tm_mon = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_mon = date.tm_mon * 10 + (date3[i] - '0');
    }

    date.tm_mon = date.tm_mon - 1;
    value = strtok(NULL, "-");
    strcpy(date3, value);
    date.tm_mday = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_mday = date.tm_mday * 10 + (date3[i] - '0');
    }

    value = strtok(date2, ":");
    strcpy(date3, value);
    date.tm_hour = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_hour = date.tm_hour * 10 + (date3[i] - '0');
    }

    value = strtok(NULL, ":");
    strcpy(date3, value);
    date.tm_min = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_min = date.tm_min * 10 + (date3[i] - '0');
    }

    value = strtok(NULL, ".");
    strcpy(date3, value);
    date.tm_sec = 0;
    n = strlen(date3);
    for (i = 0; i < n; i++) {
        date.tm_sec = date.tm_sec * 10 + (date3[i] - '0');
    }
    date.tm_isdst = 0;

// strcutura date va fi trimisa ca parametru functiei mktime
// functia mktime va calcula secundele trecute de la data crearii fisierului
    time = mktime(&date);

// se converteste numarul secundelor in octal
    dimension = 12;
    dim = conversion_octal(time, number2);
    memset(mmtime, '0', dimension);
    mmtime[dimension - 1] = '\0';

// se memoreaza la finalul vectorului mtime al headerului numarul secundelor
    j = 0;
    for (i = dimension - 2; i >= dimension - 1 - dim; i--) {
        mmtime[i] = number2[j] + '0';
        j++;
    }
}

void checksum(char charptr2[NMAX4], char chksum2[8]) {
    int i, sum, dimension2, j, dimension;
    int number2[NMAX];
    sum = 0;

// se calculeaza suma bytes-ilor
    for (i = 0; i < 512; i++) {
        sum = sum + charptr2[i];
    }

// se obtine lungimea numarului in octal ce trebuie adaugat in campul checksum
    dimension2 = conversion_octal(sum, number2);
    j = 0;
    dimension = 8;
    memset(chksum2, '0', 8);
    chksum2[7] = ' ';
    chksum2[6] = '\0';

// se memoreaza suma calculata la finalul vectorului checksum
    for (i = dimension - 3; i >= dimension - 2 - dimension2; i--) {
        chksum2[i] = number2[j] + '0';
        j++;
    }
}

// se calculeaza numarul reprezentand permisiunile fisierului
void permissions(char permission[NMAX3], char mmode[8]) {
    int i;
    int number;
    number = 0;

    for (i = 1; i <= 9; i = i + 3) {
        if (permission[i] == 'r') {
            number = number + 4;
        }
        if (permission[i + 1] == 'w') {
            number = number + 2;
        }
        if (permission[i + 2] == 'x') {
            number = number + 3;
        }

        number = number * 10;
    }

// memoreaza numarul intr-un vector char
    number = number / 10;
    mmode[4] = number / 100 + '0';
    mmode[5] = (number % 100) / 10 + '0';
    mmode[6] = number % 10 + '0';
}

// se creaza header-ul arhivei
void create_header(char permission[NMAX3], char owner_name[NMAX],
                   char owner_group[NMAX], char file_size[NMAX],
                   char date1[NMAX3], char date2[NMAX3], char name[NMAX2],
                   char nume_arhiva[NMAX2], char nume_director[NMAX2]) {
    int i, j, n, ok = 0, size2, number_blocks; char *value;
    union record fisier;
    char uid1[NMAX], gid1[NMAX], nume_director2[NMAX2], line[NMAX2], v[NMAX4];

    FILE *fin = fopen("usermap.txt", "rt"), *fout;
    if (fin == NULL) {
        exit(1);
    }

// se initializeaza campurile structurii header
    memset(&fisier, '\0', 512);
    memset(fisier.header.magic, '\0', 8);
    strcpy(fisier.header.magic, "GNUtar  ");
    fisier.header.magic[strlen(fisier.header.magic) - 1] = '\0';
    fisier.header.typeflag = '0';
    memset(fisier.header.name, '\0', 100);
    memset(fisier.header.uname, '\0', 32);
    memset(fisier.header.gname, '\0', 32);
    memset(fisier.header.devmajor, '0', 8);
    memset(fisier.header.linkname, '\0', 100);
    memset(fisier.header.mode, '0', 8); fisier.header.mode[7] = '\0';
    memset(fisier.header.chksum, ' ', 8);
    memset(fisier.header.devminor, '0', 8);
    fisier.header.devminor[7] = '\0'; fisier.header.devmajor[7] = '\0';
    strcpy(fisier.header.uname, owner_name);
    strcpy(fisier.header.gname, owner_group);
    strcpy(fisier.header.linkname, name); strcpy(fisier.header.name, name);
    owner_group[strlen(owner_group)] = '\0';
    owner_name[strlen(owner_name)] = '\0';
    fisier.header.name[strlen(fisier.header.name) - 1] = '\0';
    fisier.header.linkname[strlen(fisier.header.linkname) - 1] = '\0';
    permissions(permission, fisier.header.mode);

// se parcurge fisierul usermap pentru a gasi id-ul utilizatorului
// si id-ul grupului din care face parte
    while (fgets(line, 100, fin) && ok == 0) {
        value = strtok(line, ":");
        strcpy(owner_group, value);
        if (strcmp(owner_group, owner_name) == 0) {
            value = strtok(NULL, ":");
            value = strtok(NULL, ":");
            strcpy(uid1, value);
            value = strtok(NULL, ":");
            strcpy(gid1, value); ok = 1;
        }
    }

    timp(date1, date2, fisier.header.mtime);
// se convertesc in octal valorile reprezentand gid, uid si dimensiunea
    conversion(gid1, fisier.header.gid, 8);
    conversion(uid1, fisier.header.uid, 8);
    conversion(file_size, fisier.header.size, 12);
    checksum(fisier.charptr, fisier.header.chksum);

    fclose(fin);
// se deschide noua arhiva
    fout = fopen(nume_arhiva, "ab");
    if (fout == NULL) {
        exit(1);
    }
    strcpy(nume_director2, nume_director);
    nume_director2[strlen(nume_director2)-1] = '\0';
    strcat(nume_director2, name);
    nume_director2[strlen(nume_director2)-1] = '\0';
    fwrite(&fisier.charptr, sizeof(fisier.charptr), 1, fout);
    fin = fopen(nume_director2, "rb");
    if (fin == NULL) {
        printf("> Failed!\n");
        exit(1);
    }

    size2 = file_size[0] - '0'; n = strlen(file_size);

// se memoreaza dimensiunea fisierului intr-o variabila de tip int
    for (i = 1; i < n; i++) {
        size2 = size2 * 10 + (file_size[i] - '0');
    }
// se calculeaza cate blocuri de 512 de bytes contine fisierul
    if (size2 % 512 == 0) {
        number_blocks = size2 / 512;
    } else {
          number_blocks = size2 / 512 + 1;
      }

// se citesc cate 512 de bytes din fisier si se adauga in arhiva
    for (i = 0; i < number_blocks; i++) {
        for (j = 0; j < 512; j++) {
            v[j] = '\0';
        }

        fread(v, 1, 512, fin);
        fwrite(v, 1, 512, fout);
    }

    fclose(fin); fclose(fout);
}

void list_archive(char nume_arhiva[NMAX2]) {
    int i, number_bytes, number_blocks, j, size, n, size2;
    char name_file[NMAX2], number[12];
    FILE *fin;

    nume_arhiva[strlen(nume_arhiva) - 1] = '\0';
    fin = fopen(nume_arhiva, "rb");

    if (fin == NULL) {
        printf("File not found\n");
        exit(1);
    } else {
// se calculeaza dimensiunea arhivei
         fseek(fin, 0, SEEK_END);
         number_bytes = ftell(fin);
         number_blocks = number_bytes / 512;

// se plaseaza cursorul la inceputul arhivei
        fseek(fin, 0, SEEK_SET);

// se va parcurge fiecare parte a arhivei ce corespunde unui anumit fisier
        for (i = 0; i < number_blocks; i++) {
// se citeste numele fisierului si se intoarce cursorul la inceputul partii
            fgets(name_file, 100, fin);
            fseek(fin, -99, SEEK_CUR);
// se muta cursorul spre partea de header ce contine dimensiunea fisierului
            fseek(fin, 124, SEEK_CUR);
            fgets(number, 12, fin);

            size2 = 0;
            n = strlen(number);
// se memoreaza dimensiunea fisierului intr-o variabila de tip int
            for (j = 1; j < n; j++) {
                size2 = size2 * 10 + (number[j] - '0');
            }
// se converteste valoarea dimensiunii din octal in zecimal
            size2 = conversion_decimal(size2);

// se calculeaza numarul de blocuri de cate 512 bytes
            if (size2 % 512 == 0) {
                size = size2 / 512;
            } else {
                    size = size2 / 512 + 1;
              }

            fseek(fin, -135, SEEK_CUR);
// cursorul va sari peste portiunea arhivei ce contine informatii despre fisier
            number_bytes = 512 * (size + 1);

            fseek(fin, number_bytes, SEEK_CUR);
// daca aceasta parte din arhiva corespunde unui fisier se afiseaza numele lui
// sau in cazul in care fisierul este gol si are dimensiunea 0
            if (size2 != 0 || (size2 == 0 && name_file[0] != '\0')) {
                printf("> %s\n", name_file);
            }
    }
  }
    fclose(fin);
}

void extract_file(char nume_fisier[NMAX2], char nume_arhiva[NMAX2]) {
    int i, k, number_bytes, number_blocks, j, ok;
    ok = 0;
    char name_file[NMAX2], number[12], v[NMAX4], new_file[NMAX2];
    FILE *fin, *fout;

    nume_arhiva[strlen(nume_arhiva) - 1] = '\0';
    fin = fopen(nume_arhiva, "rb");

    if (fin == NULL) {
        printf("> File not found!\n");
        exit(1);
    } else {
// se calculeaza dimensiunea arhivei
        fseek(fin, 0, SEEK_END);
        number_bytes = ftell(fin);
        number_blocks = number_bytes / 512;

// se plaseaza cursorul la inceputul arhivei
        fseek(fin, 0, SEEK_SET);

// se va parcurge fiecare parte a arhivei ce corespunde unui anumit fisier
        for (i = 0; i < number_blocks && ok == 0; i++) {
// se citeste numele fisierului si se intoarce cursorul la inceputul partii
            fgets(name_file, 100, fin);
            fseek(fin, -99, SEEK_CUR);

// se muta cursorul spre partea de header ce contine dimensiunea fisierului
            fseek(fin, 124, SEEK_CUR);
            fgets(number, 12, fin);

            int size, n, size2;
            size2 = 0;
            n = strlen(number);

// se memoreaza dimensiunea fisierului intr-o variabila de tip int
            for (j = 1; j < n; j++) {
                size2 = size2 * 10 + (number[j] - '0');
            }

// se converteste valoarea dimensiunii din octal in zecimal
            size2 = conversion_decimal(size2);

// se calculeaza numarul de blocuri de cate 512 bytes
            if (size2 % 512 == 0) {
                size = size2 / 512;
            } else {
                size = size2 / 512 + 1;
              }

            fseek(fin, -135, SEEK_CUR);

// daca numele citit este cel al fisierului cerut se va dezarhiva acesta
            if (strcmp(name_file, nume_fisier) == 0) {
                name_file[strlen(name_file)] = '\0';
// se deschide noul fisier
                strcpy(new_file, "extracted_");
                strcat(new_file, name_file);
                fout = fopen(new_file, "wb");
                if (fout == NULL) {
                    exit(1);
                }

// cursorul se muta la continutul fisierului din arhiva
                fseek(fin, 512, SEEK_CUR);
// se citesc cate 512 de bytes si se scriu in noul fisier
                for (j = 0; j < size - 1; j++) {
                    for (k = 0; k < 512; k++) {
                        v[k] = '\0';
                    }
                    fread(v, 1, 512, fin);
                    fwrite(v, 1, 512, fout);
                }

// se citesc octetii ramasi, acestia nefiind multiplu de 512
                number_bytes = size2 % 512;
                fread(v, 1, number_bytes, fin);
                fwrite(v, 1, number_bytes, fout);
                fclose(fout);

                i = number_blocks;
                ok = 1;
          }
// daca nu este fisier dorit cursorul trece la urmatorul fisier
          number_bytes = 512 * (size + 1);
          fseek(fin, number_bytes, SEEK_CUR);
        }
    fclose(fin);
      }
    if (ok == 1) {
        printf("> File extracted!\n");
    } else {
        printf("> File not found!\n");
      }
}

void create_file(char nume_director[NMAX2], char nume_arhiva[NMAX2]) {
    char permission[NMAX3], file_size[NMAX], file_size2[NMAX], date1[NMAX3],
        date2[NMAX3], owner_group[NMAX];
    char owner_name[NMAX], *value, name[NMAX2], line[NMAX2];
    FILE *in, *ffout;
    in = fopen("files.txt", "rt");
    if (in == NULL) {
        printf("> File not found!\n");
        exit(10);
    }
// se parcurge fisierul files.txt si se adauga in arhiva fiecare fisier
    while (fgets(line, 100, in)) {
        value = strtok(line, " ");
        strcpy(permission, value);
        value = strtok(NULL, " ");
        value = strtok(NULL, " ");
        strcpy(owner_name, value);
        value = strtok(NULL, " ");
        strcpy(owner_group, value);
        value = strtok(NULL, " ");
        strcpy(file_size, value);

        strcpy(file_size2, file_size);
        value = strtok(NULL, " ");
// se retine data crearii fisierului
        strcpy(date1, value);
        value = strtok(NULL, " ");
// se retine ora crearii fisierului
        strcpy(date2, value);
        value = strtok(NULL, " ");
        value = strtok(NULL, " ");
        strcpy(name, value);

        create_header(permission, owner_name,
                    owner_group, file_size2, date1, date2, name,
                    nume_arhiva, nume_director);
    }
    ffout = fopen(nume_arhiva, "ab");
    char v[NMAX4];
    int j;

    for (j = 0; j < 512; j++) {
        v[j] = '\0';
    }
// finalul arhivei contine o zona de 512 bytes de '\0'

    if (ffout == NULL)  {
        printf("> Failed!\n");
        exit(10);
    }  else {
            fwrite(&v, 1, 512, ffout);
            printf("> Done!\n");
       }
    fclose(in);
    fclose(ffout);
}

int main() {
    char command[511], nume_director[NMAX2], nume_arhiva[NMAX2];
    char *name2, *create, *list, *extract, nume_fisier[NMAX2];
    int ok;

    while (fgets(command, 511, stdin)) {
        ok = 0;
        if (ok == 0) {
            if (strstr(command, "exit") != NULL) {
                return 0;
            }

// daca se primeste comanda create
            create = strstr(command, "create");
            if (create != NULL) {
                create = strtok(command, " ");
                name2 = strtok(NULL, " ");

                if (name2 == NULL) {
                    printf("> Wrong command!\n");
                } else {
                    strcpy(nume_arhiva, name2);
                    name2 = strtok(NULL, " ");

                    if (name2 == NULL) {
                        printf("> Wrong command!\n");
                    } else {
                        create = strtok(NULL, " ");

                        if (create != NULL) {
                            printf("> Wrong command!\n");
                        } else {strcpy(nume_director, name2);
                                create_file(nume_director, nume_arhiva);
                                ok = 1;
                                }
                        }
                    }
            }

// daca se primeste comanda list
            list = strstr(command, "list");
            if (list != NULL) {
                list = strtok(command, " ");
                name2 = strtok(NULL, " ");

                if (name2 == NULL) {
                    printf("> Wrong command!");
                } else {
                    strcpy(nume_arhiva, name2);
                    list_archive(nume_arhiva);
                    }
            }

// daca se primeste comanda extract
            extract = strstr(command, "extract");
            if (extract != NULL) {
                extract = strtok(command, " ");

                if (extract == NULL) {
                    printf("> Wrong command!");
                } else {
                    name2 = strtok(NULL, " ");

                    if (name2 == NULL) {
                        printf("> Wrong command!");
                    } else {
                        strcpy(nume_fisier, name2);
                        name2 = strtok(NULL, " ");

                        if (name2 == NULL) {
                            printf("> Wrong command!");
                        } else {
                            strcpy(nume_arhiva, name2);
                            extract_file(nume_fisier, nume_arhiva);
                          }
                      }
                }
            }
        }

// daca nu se primeste o comanda cunoscuta
        if ((strstr(command, "exit") == NULL &&
            strstr(command, "create") == NULL &&
            strstr(command, "list") == NULL &&
            strstr(command, "extract") == NULL) &&
            command[0] != '\n')
            printf("> Wrong command!");
    }

    return 0;
}
