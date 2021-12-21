//
// Created by Boris Jurič on 08.12.2021.
//

#ifndef MAIN_CPP_ALIGNER_H
#define MAIN_CPP_ALIGNER_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <utility>
#include <memory>
#include <chrono>
#include "string"

#define GAP 1
#define MATCH 2

struct Element
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

class Aligner {
private:
    void make_matrix(uint32_t columns, uint32_t rows);
    void horizontal_gap(int i, int j, int bestscore);
    void vertical_gap(int i, int j, int bestscore);
    Element* cruise(int i, int j, int score);
    int topscore;
    std::string* targetSequence;
    std::string* querySequence;
    std::vector<std::vector<Element> > Matrix;
    std::unordered_map <int, std::deque<struct Element*> > MapOfPath;
    uint32_t targetLen, queryLen;
public:
    Aligner();
    Element* align(std::string &t, std::string &q);
    void print_alignment(Element *&element);
    void clean_matrix();
};


#endif //MAIN_CPP_ALIGNER_H
