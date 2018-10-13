# COMP20003 Algorithms and Data Structures Second (Spring) Semester 2018 [Assignment 2]
# Solving Puzzle Games Optimally: Graph Search

## Purpose
The purpose of this assignment is for you to:
* Increase your proficiency in C programming, your dexterity with efficient memory allocation and your algorithmic thinking, through programming a search algorithm over Graphs.
* Practice your ability to understand new algorithms based on those already taught in class.
* Gain experience with applications of graphs and graph algorithms to create an AI solver for a game.
* Foster your capability to understand a scientific paper and implement its ideas (optional).

## Assignment description
In this programming assignment you’ll be expected to build a solver for the 15–puzzle https://enwikipedia.org/wiki/15_puzzle.

## The 15–puzzle

These puzzles consist in assembling an image or some geometrical patterns, which has been decomposed into a number of sliding tiles. We can consider each tile to be identified by a number, so we can represent the initial state of the puzzle with the following diagram:

1 2 7 6

3 4 8 5

9 10 11 12

B 14 15 13

where B is a blank space. A puzzle solution is a sequence of moves which achieves the following state

B 1 3 2

4 5 6 7

8 9 10 11

12 13 14 15

subject to the constraint that we can only swap positions of the blank tile with some adjacent one.
For instance, in the initial state depicted above, the only valid moves would be:
* Swap positions of tile 9 and blank B.
* Swap positions of tile 14 and blank B.
Each possible configuration of the puzzle is called a state. The 15-puzzle Graph G = hV, Ei is implicitly defined. The vertex set V is defined as all the possible puzzle configurations (states), and the edges E connecting two vertexes are defined by the legal movements, also called actions or operators.

## Algorithm
Your solver should contain the implementation of the following search algorithm:

1. Iterative Deepening A∗ (IDA∗)

The algorithm follows the Depth-First Search search strategy and can be easily implemented as a
recursive function.

IDA∗ has two parts:

* The main loop, initializes the thresholds B and B0 first. If no solution is found the search is
triggered again but with an updated B = B0 threshold.

* The recursive function that implements the bounded Depth-First Search.

In this assignment IDA∗ is guaranteed to find the optimal solution. In general, IDA∗ returns optimal solutions as long as the heuristic function is a lower bound on the true optimal solution length (see next section for more information on the heuristic used in this assignment).
