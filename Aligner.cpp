//
// Created by Boris Jurič on 08.12.2021.
//
#include "Aligner.h"

Aligner::Aligner()= default;

void Aligner::make_matrix(uint32_t columns, uint32_t rows)
{
    Matrix.reserve(columns);
    for (int i = 0; i < columns; i++)
    {
        Matrix.emplace_back();
        Matrix[i].reserve(rows);
        int offset = targetLen - i;
        for (int j = 0; j < rows; j ++)
        {
            Matrix[i].emplace_back(0, i, j);
            if (offset < queryLen)
            {
                MapOfPath[topscore - ((queryLen-offset)+j)*MATCH].push_back(&Matrix[i][j]);
            }
            else
            {
                MapOfPath[topscore - (j)*MATCH].push_back(&Matrix[i][j]);
            }
        }
    }
}

void Aligner::clean_matrix()
{
    std::cout<<"Cleaning matrix"<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < Matrix.size(); i++)
    {
        for (int j = 0; j < Matrix[i].size(); j++)
        {
            Matrix[i][j].score = 0;
            Matrix[i][j].previous = nullptr;
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout<<"Matrix cleaned in: " << duration.count() <<" ms"<<std::endl;
}

void Aligner::horizontal_gap(int i, int j, int bestscore)
{
    int penalty = 0;                 //if we move to shorter diagonal or just make gaps
    int score = Matrix[i][j].score;

    //std::cout<<"making gap at "<< i << ", " << j <<" ";

    for (int jump = 2;jump < Matrix[i][j].score+1;jump++)
    {
        score -= GAP;

        if ((score <= 0)||(i + jump >= targetLen - 1))
            break;

        penalty += GAP;

        if ((*targetSequence)[i + jump] == (*querySequence)[j+1])
        {
            if (Matrix[i + jump][j+1].previous)
            {
                if (Matrix[i][j].score <= Matrix[i + jump][j+1].score)
                    break;
            }
            //std::cout<<"at "<<i + jump<<" "<<j<<" : "<<(*targetSequence)[i + jump]<<" "<<(*querySequence)[j+1]<<" ";
            Matrix[i + jump][j+1].previous = &Matrix[i][j];
            Matrix[i + jump][j+1].score = score + MATCH;

            if (targetLen - (i + jump - (j+1)) < queryLen)
                penalty += MATCH;
            //std::cout<<"query score = "<<bestscore - penalty<<std::endl;
            MapOfPath[bestscore - penalty].push_front(&Matrix[i + jump][j+1]);

            return;                                  //this here prevents factorial generation of children paths
        }
    }
}

void Aligner::vertical_gap(int i, int j, int bestscore)
{
    int penalty = 0;
    int score = Matrix[i][j].score;

    for (int jump = 2; jump < Matrix[i][j].score+1; jump++)
    {
        score -= GAP;

        if ((score <= 0)||(j + jump >= queryLen - 1))
            break;

        penalty += GAP+MATCH;

        if ((*targetSequence)[i+1] == (*querySequence)[j + jump])
        {
            if (Matrix[i+1][j + jump].previous)
            {
                if (Matrix[i][j].score <= Matrix[i+1][j + jump].score)
                    break;
            }
            //std::cout<<"at "<<i+1<<" "<<j + jump<<" : "<<(*targetSequence)[i+1]<<" "<<(*querySequence)[j + jump]<<" ";
            Matrix[i+1][j + jump].previous = &Matrix[i][j];
            Matrix[i+1][j + jump].score = score + MATCH;

            //if (abs((element.column - element.row)) < abs((element.column - element.row + jump)))
            MapOfPath[bestscore - penalty].push_front(&Matrix[i+1][j + jump]);

            return;                                  //this here prevents factorial generation of children paths
        }
    }
}

Element* Aligner::cruise(int i, int j, int score)
{
    while((*targetSequence)[i] == (*querySequence)[j])
    {
        if (Matrix[i][j].score == score)
            return &Matrix[i][j];

        if ((i + 1 >= targetLen) || (j + 1 >= queryLen))
        {
            MapOfPath[Matrix[i][j].score].push_front(&Matrix[i][j]);
            return nullptr;
        }

        vertical_gap(i, j, score);
        horizontal_gap(i, j, score);

        if ((*targetSequence)[i+1] == (*querySequence)[j+1])
        {
            if (Matrix[i + 1][j + 1].previous)
            {
                if (Matrix[i + 1][j + 1].score < Matrix[i][j].score + MATCH)
                {
                    Matrix[i + 1][j + 1].score = Matrix[i][j].score + MATCH;
                    Matrix[i + 1][j + 1].previous = &Matrix[i][j];
                }
                else
                    return nullptr;
            }
            else
            {
                Matrix[i + 1][j + 1].score = Matrix[i][j].score + MATCH;
                Matrix[i + 1][j + 1].previous = &Matrix[i][j];
            }
        }
        else if (Matrix[i][j].score - MATCH > 0)
        {
            int current = Matrix[i][j].score;
            int penalty = 0;
            auto previous = &Matrix[i][j];

            i += 2; j += 2;

            while (current - penalty > 0)
            {
                if ((i >= queryLen) || (j >= targetLen))
                    return nullptr;
                if ((*targetSequence)[i] == (*querySequence)[j])
                {
                    if (Matrix[i][j].previous)
                    {
                        if (Matrix[i][j].score < current - penalty)
                        {
                            Matrix[i][j].score = current - penalty;
                            Matrix[i][j].previous = previous;
                            //std::cout<<"pushing "<<i<<" "<<j<<" to : "<<score - (penalty + MATCH*2)<<" with score: "<<current - penalty;
                            MapOfPath[score - (penalty + MATCH*2)].push_front(&Matrix[i][j]);
                            return nullptr;
                        }
                        else
                            return nullptr;
                    }
                    else
                    {
                        Matrix[i][j].score = current - penalty;
                        Matrix[i][j].previous = previous;
                        MapOfPath[score - (penalty + MATCH*2)].push_front(&Matrix[i][j]);
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

void Aligner::print_alignment(Element *&element)
{
    std::deque<std::pair<int,int> > finalAlignment;
    finalAlignment.push_front({element->column,element->row});
    auto current = element->previous;
    while (current)
    {
        finalAlignment.push_front({current->column,current->row});
        current = current->previous;
    }
    int pos = finalAlignment[0].first;
    std::string tempcigar;
    std::string cigar;
    std::cout<<"pos : "<<pos;
    for (auto pair : finalAlignment)
    {
        std::cout << pair.first << " " << pair.second << "; " ;
        if ((*targetSequence)[pair.first] == (*querySequence)[pair.second])
        {
            if (pair.first - pos == pair.second) {
                if (tempcigar.back() == 'M' || tempcigar.empty())
                    tempcigar += "M";
                else
                {
                    cigar += std::to_string(std::count(tempcigar.begin(), tempcigar.end(), tempcigar[0]));
                    cigar += tempcigar[0];
                    tempcigar = "M";
                }
            } else if (pair.first - pos > pair.second) {
                pos++;
                if (tempcigar.back() == 'I')
                    tempcigar += 'I';
                else
                {
                    cigar += std::to_string(std::count(tempcigar.begin(), tempcigar.end(), tempcigar[0]));
                    cigar += tempcigar[0];
                    tempcigar = "I";
                }
            }
            else {
                pos--;
                if (tempcigar.back() == 'D')
                    tempcigar += 'D';
                else
                {
                    cigar += std::to_string(std::count(tempcigar.begin(), tempcigar.end(), tempcigar[0]));
                    cigar += tempcigar[0];
                    tempcigar = "D";
                }
            }
        }
        else{
            if (tempcigar.back() == 'N')
                tempcigar += 'N';
            else
            {
                cigar += std::to_string(std::count(tempcigar.begin(), tempcigar.end(), tempcigar.back()));
                cigar += tempcigar[0];
                tempcigar = "N";
            }
        }
    }
    cigar += std::to_string(std::count(tempcigar.begin(), tempcigar.end(), tempcigar[0]));
    cigar += tempcigar[0];

    std::cout<<std::endl<<cigar<<std::endl;
}

Element* Aligner::align(std::string &t, std::string &q)
{
    if (t.size() >= q.size())
    {
        targetSequence = &t;
        querySequence = &q;
    }
    else
    {
        targetSequence = &q;
        querySequence = &t;
    }

    targetLen = (*targetSequence).size(); queryLen = (*querySequence).size();
    topscore = MATCH*queryLen;

    std::cout<<"Making matrix"<<std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    make_matrix(targetLen, queryLen);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::seconds>(stop - start);
    std::cout<<"Matrix built in: " << duration.count() <<" s"<<std::endl;

    std::cout<<"Starting alignment..."<<std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = topscore; i > 0; i--)
    {
        //std::cout << "searching score: " << i<<std::endl;
        while (!MapOfPath[i].empty())
        {
            //std::cout<<std::endl<<"element: "<<MapOfPath[i].front()->column<<" "<<MapOfPath[i].front()->row<<std::endl;
            if (!Matrix[MapOfPath[i].front()->column][MapOfPath[i].front()->row].previous)
                Matrix[MapOfPath[i].front()->column][MapOfPath[i].front()->row].score += MATCH;
            auto last = cruise(MapOfPath[i].front()->column, MapOfPath[i].front()->row, i);
            if (last)
            {
                stop = std::chrono::high_resolution_clock::now();
                auto duration = duration_cast<std::chrono::microseconds>(stop - start);
                std::cout<<"Alignment found in: " << duration.count() <<" us"<<std::endl;
                return last;
            }
            MapOfPath[i].pop_front();
        }
    }
    return nullptr;
}