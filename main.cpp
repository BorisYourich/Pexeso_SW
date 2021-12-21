#include <iostream>
#include <vector>
#include "set"
#include <fstream>
#include "string"
#include "Aligner.h"

//#define targetLen 16
//#define queryLen 16
//#define TOPSCORE queryLen*2
/*#define GAP 1
#define MATCH 2*/

/*int TARGET = T.size();
int QUERY = Q.size();

int TOPSCORE = QUERY*2;*/
/*struct Element
{
    Element(int s, int c, int r)
    {
        score = s; column = c; row = r; previous = nullptr;
    }
    int score;
    int column;
    int row;
    Element* previous;
};
std::vector<std::vector<Element>> Matrix;
std::unordered_map <int, std::queue<struct Element*>> MapOfPath;*/

/*void make_matrix(int columns, int rows)
{
    Matrix.reserve(columns);
    for (int i = 0; i < columns; i++)
    {
        Matrix.emplace_back();
        Matrix[i].reserve(rows);
        for (int j = 0; j < rows; j ++)
        {
            Matrix[i].emplace_back(0, i, j);
            if (TARGET - i < QUERY)
                MapOfPath[TOPSCORE - (i+j)*2].push(&Matrix[i][j]);      //nonsens, FIXTHIS!!!!
            else
                MapOfPath[TOPSCORE - (j)*2].push(&Matrix[i][j]);
        }
    }
}

void horizontal_gap(Element &element)
{
    int penalty = 0;                 //if we move to shorter diagonal or just make gaps
    int score = element.score;

    for (int jump = 2;jump<element.score;jump++)
    {
        score -= GAP;

        if ((score <= 0)||(element.column + jump >= TARGET - 1))
            break;

        penalty += GAP;

        if (T[element.column + jump] == Q[element.row])
        {
            if (Matrix[element.column + jump][element.row].previous)
            {
                if (element.score <= Matrix[element.column+jump][element.row].score)
                    break;
            }

            Matrix[element.column + jump][element.row].previous = &element;
            Matrix[element.column + jump][element.row].score = score + MATCH;

            if (abs((element.column - element.row)) < abs((element.column + jump - element.row)))
                penalty += MATCH;
            MapOfPath[element.score - penalty].push(&Matrix[element.column + jump][element.row]);

            return;                                  //this here prevents factorial generation of children paths
        }
    }
}
void vertical_gap(Element &element)
{
    int penalty = 0;
    int score = element.score;

    for (int jump = 2; jump < element.score; jump++)
    {
        score -= GAP;

        if ((score <= 0)||(element.row + jump >= QUERY - 1))
            break;

        penalty += GAP;

        if (T[element.column] == Q[element.row + jump])
        {
            if (Matrix[element.column ][element.row + jump].previous)
            {
                if (element.score <= Matrix[element.column][element.row + jump].score)
                    break;
            }

            Matrix[element.column][element.row + jump].previous = &element;
            Matrix[element.column][element.row + jump].score = score + MATCH;

            if (abs((element.column - element.row)) < abs((element.column - element.row + jump)))
                penalty += MATCH;
            MapOfPath[element.score - penalty].push(&Matrix[element.column][element.row + jump]);

            return;                                  //this here prevents factorial generation of children paths
        }
    }
}

Element* cruise(int i, int j, int score)
{
    while(T[j] == Q[i])
    {
        Matrix[i][j].score += MATCH;

        if (Matrix[i][j].score == score)
            return &Matrix[i][j];

        if ((i + 1 >= QUERY) || (j + 1 >= TARGET))
        {
            MapOfPath[score - 1].push(&Matrix[i][j]);
            return nullptr;
        }

        vertical_gap(Matrix[i][j]);

        if (T[j+1] == Q[i+1])
        {
            if (Matrix[i + 1][j + 1].previous)
            {
                if (Matrix[i + 1][j + 1].score < Matrix[i][j].score + MATCH)
                {
                    Matrix[i + 1][j + 1].score = Matrix[i][j].score;
                    Matrix[i + 1][j + 1].previous = &Matrix[i][j];
                    horizontal_gap(Matrix[i][j]);
                }
                else
                    return nullptr;
            }
            else
            {
                Matrix[i + 1][j + 1].score = Matrix[i][j].score;
                Matrix[i + 1][j + 1].previous = &Matrix[i][j];
                //std::cout<<i<<","<<j<<" <- "<<i+1<<","<<j+1<<std::endl;
                horizontal_gap(Matrix[i][j]);
            }
        }
        else if (Matrix[i][j].score - MATCH > 0)
        {
            int penalty = MATCH;
            while (Matrix[i][j].score - penalty > 0)
            {
                if ((i + 2 >= QUERY) || (j + 2 >= TARGET))
                    return nullptr;
                if (T[j+2] == Q[i+2])
                {
                    if (Matrix[i + 2][j + 2].previous)
                    {
                        if (Matrix[i + 2][j + 2].score < Matrix[i][j].score - penalty+MATCH)
                        {
                            Matrix[i + 2][j + 2].score = Matrix[i][j].score - penalty+MATCH;
                            MapOfPath[score - penalty+MATCH].push(&Matrix[i + 2][j + 2]);
                            return nullptr;
                        }
                        else
                            return nullptr;
                    }
                    else
                    {
                        Matrix[i + 2][j + 2].score = Matrix[i][j].score - penalty+MATCH;
                        MapOfPath[score - penalty+MATCH].push(&Matrix[i + 2][j + 2]);
                        return nullptr;
                    }
                }
                i++;j++;
                penalty += MATCH;
            }
            return nullptr;
        }
        i++;j++;
    }
    return nullptr;
}

Element* align()
{
    for (int i = TOPSCORE; i > 0; i--)
    {
        std::cout << "searching score: " << i << std::endl;
        while (!MapOfPath[i].empty())
        {
            auto last = cruise(MapOfPath[i].front()->column, MapOfPath[i].front()->row, i);
            if (last)
                return last;
            MapOfPath[i].pop();
        }
    }
    return nullptr;
}

void print_alignment(Element *&element)
{
    std::deque<std::pair<int,int> > finalAlignment;
    finalAlignment.push_front({element->column,element->row});
    auto current = element->previous;
    while (current)
    {
        finalAlignment.push_front({current->column,current->row});
        current = current->previous;
    }
    for (auto pair : finalAlignment)
        std::cout << pair.first << " " << pair.second << "; " ;
}*/

int main() {
                            //https://www.ncbi.nlm.nih.gov/genome/?term=Escherichia+coli+strain+536
    std::string refrence = "/Users/borisjuric/Desktop/Pexeso SW/Escherichia_coli.fna";
    std::string row;
    std::string T; //= "AGTAACTGACTGACTGA";
    //Q = "ACTGACTGACTGACTG";
    //               TGATATTGAAAAAAATATCACCAAATAAAAAACGCCTTAGTAAGTATTTTTC
    std::string Q = "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATAGCGCACAGAC"; //= "ATAACTGACTGACTGA";
    std::ifstream file (refrence);
    if (file.is_open()) {
        getline(file, row);
        while (!file.eof()) {
            getline(file, row);
            T += row;
        }
    }
    else {
        std::cout << "Error opening file!" << std::endl;
        return 1;
    }


    Aligner aligner;

    auto result = aligner.align(T, Q);

    aligner.print_alignment(result);

    aligner.clean_matrix();








    /*std::vector<char> AA {'A','R','N','D','C','Q','E','G','H','I','L','K','M','F','P','S','T','W','Y','V'};
    std::set<char> combinations;

    for (int i = 0; i < AA.size(); i++)
    {
        std::cout<<AA[i]<< " | "<<std::endl;
        for(int j = i+1; j < AA.size(); j++ )
        {
            auto out = (int)AA[i] * (int)AA[j];
            if (combinations.contains(out))
                std::cout<<AA[j] << " "<< out<<' ';
            combinations.insert(out);
            //std::cout<< AA[j] << "=" << out <<' ';
        }
        std::cout<<std::endl;
    }*/


    /*std::bitset<32> targetSequence;
    std::bitset<32> tA;
    std::bitset<32> tC;
    std::bitset<32> tG;
    std::bitset<32> tT;*/

    /*if (TARGET < QUERY)
    {
        std::swap(TARGET, QUERY);
        std::swap(T, Q);
    }

    make_matrix(TARGET, QUERY);
    auto bestAlignment = align();

    print_alignment(bestAlignment);*/



    /*convert(t, targetSequence);
    std::cout<< targetSequence << std::endl << std::endl;

    compare(targetSequence, tA, tC, tG, tT);
    std::cout << "compares" << std::endl << tA << std::endl << tC << std::endl << tG << std::endl << tT << std::endl;

    build_matrix(q, tA, tC, tG, tT);

    std::cout << "Matrix:" << std::endl;
    for (auto bits : matrix)
    {
        for (int i = 0; i < TOPSCORE; i+=2)
            std::cout << bits[i] <<"   ";
        std::cout << std::endl;
    }

    initialize_map();

    auto bestAlignment = alignment();
    print_alignment(bestAlignment);*/

    return 0;
}
