# Progetto-API-2022
## [Algorithms and Data Structures](https://www4.ceda.polimi.it/manifesti/manifesti/controller/ManifestoPublic.do?EVN_DETTAGLIO_RIGA_MANIFESTO=EVENTO&c_insegn=086067&aa=2021&k_cf=225&k_corso_la=358&ac_ins=0&k_indir=IT1&lang=EN&tipoCorso=ALL_TIPO_CORSO&semestre=2&idItemOfferta=155412&idRiga=268925&codDescr=086067) 2022, Course Final Project: WordChecker

The purpose was to make a system that, at its heart, checks the matching between the letters of two words of equal length. The program is loosely inspired by the game Wordle.

Final Grade: 30 with Honors/30

Academic Year: 2021-2022

- [Project Description](#project-description)
- [Implementation](#implementation)
- [Tools Used](#tools-used)


## Project Description
In each game, there is a secret word to uncover. Following each guess, a symbol sequence is displayed, consisting of +, |, or /. These symbols convey information about each character in the guess. The + symbol indicates a correct character in the right position, the | symbol signifies a correct character in the wrong position, and the / symbol denotes a character that is not present in the secret word at all.

The project is evaluated on memory efficiency and speed.

# Implementation

In my approach, I implemented a Binary Search Tree **(BST)** in conjunction with a list. Whenever a word is entered, it is stored within a node in the BST. The list is employed to efficiently navigate through only those words that meet the given constraints. Following each attempt, if I acquire new constraints, I discard the words that no longer adhere to these updated criteria.
The use of a list within the BST allows to save efficiently memory.

```c
typedef struct Node{
    struct node_* left;
    struct node_* right;
    struct node_* next;
    char* w;
}BSTNode;
```


For the costraints I've used **matrixes**, **arrays** and **custom types**

## Tools used
- Valgrind;
- Callgrind;
- Massif-Visualizer;
- Address-Sanitizer;
- GDB;
- GCC.
- Clion;
- Visual Studio Code.

