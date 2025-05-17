#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
using namespace std;

struct Node {
    char letter;
    int freq;
    Node* l, * r;

    Node(char l, int f) :
        letter(l),
        freq(f),
        l(nullptr),
        r(nullptr){}
};


//структура минимальной кучи
struct Min_Stack{
    int size;
    vector<Node*> stack;

    Min_Stack(int s) : size(s), stack(s) {}
};

//Реализация Heapify (функция для создания кучи)
void Heapify(Min_Stack* heap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap->size && heap->stack[left]->freq < heap->stack[smallest]->freq)
        smallest = left;

    if (right < heap->size && heap->stack[right]->freq < heap->stack[smallest]->freq)
        smallest = right;

    if (smallest != i) {
        swap(heap->stack[i], heap->stack[smallest]);
        Heapify(heap, smallest);
    }
}

//Создание минимальной кучи
Min_Stack* CreateMin_stack(char arr[], int freq[], int uniq_size)
{
    int i;

    Min_Stack* Heap = new Min_Stack(uniq_size);

    //Создаем массив указателей в минимальной куче
    //Указатели указывают на следующие элементы и частоту

    for (i = 0; i < uniq_size; ++i) {
        Heap->stack[i] = new Node(arr[i], freq[i]);
    }

    int n = Heap->size - 1;
    for (i = (n - 1) / 2; i >= 0; --i) {
        Heapify(Heap, i);
    }

    return Heap;
}

//Создает новый узел с заданным символом и частотой
Node* newNode(char letter, int freq) {
    Node* node = new Node(letter, freq);
    node->l = nullptr;
    node->r = nullptr;
    return node;
}

Node* extractMinFrom_Heap(Min_Stack* heap) {
    if (heap->size == 0) return nullptr;
    //Берём корень (минимальный элемент)
    Node* minNode = heap->stack[0];
    // Ставим последний элемент на место корня
    heap->stack[0] = heap->stack[heap->size - 1];
    heap->size--;
    // Восстанавливаем кучу
    Heapify(heap, 0);

    return minNode;
}

//Вставка элемента в кучу
void insertInto_Heap(Min_Stack* heap, Node* node) {
    heap->size++;
    int i = heap->size - 1;

    // Поднимаем узел вверх, пока он меньше родителя
    while (i > 0 && node->freq < heap->stack[(i - 1) / 2]->freq) {
        heap->stack[i] = heap->stack[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->stack[i] = node;
}

//Создание дерева Хаффмана
struct Node* CreateHuffmanTree(char arr[], int freq[],
    int uniq_size, Min_Stack* Heap) {
    struct Node *l, *r, *top;
    while (!(Heap->size == 1)) {
        l = extractMinFrom_Heap(Heap);
        r = extractMinFrom_Heap(Heap);
        top = newNode('$', l->freq + r->freq);
        top->l = l;
        top->r = r;
        insertInto_Heap(Heap, top);
    }

    return extractMinFrom_Heap(Heap);
}

typedef struct code {
    char k;
    int l;
    int code_arr[16];
    struct code* p;
} code;

typedef struct Tree {
    char g;
    int len;
    int dec;
    struct Tree* f;
    struct Tree* r;
} Tree;

bool isLeaf(Node* node) {
    return (node->l == nullptr && node->r == nullptr);
}

code* front = nullptr;
code* rear = nullptr;
int k = 0;

//Преобразует бинарный код (массив 0 и 1) в десятичное число.
int convertBinaryToDecimal(int binary[], int length) {
    int decimal = 0;
    for (int i = 0; i < length; i++) {
        decimal = (decimal << 1) | binary[i]; // Эквивалентно decimal = decimal * 2 + binary[i]
    }
    return decimal;
}

//Рекурсивная функция для создания спрессованного файла
void printCodeIntoFile(ofstream& file, Node* root, int t[], int top = 0) {
    int i;
    if (root->l) {
        t[top] = 0;
        printCodeIntoFile(file, root->l, t, top + 1);
    }
    if (root->r) {
        t[top] = 1;
        printCodeIntoFile(file, root->r, t, top + 1);
    }
    if (isLeaf(root)) {
        code* data = (code*)malloc(sizeof(code));
        Tree* tree = (Tree*)malloc(sizeof(Tree));
        data->p = NULL;
        data->k = root->letter;
        tree->g = root->letter;
        file.write(&tree->g, sizeof(char));

        for (i = 0; i < top; i++) {
            data->code_arr[i] = t[i];
        }

        tree->len = top;
        file.write(reinterpret_cast<char*>(&tree->len), sizeof(int));
        
        tree->dec = convertBinaryToDecimal(data->code_arr, top);
        file.write(reinterpret_cast<char*>(&tree->dec), sizeof(int));
        data->l = top;
        data->p = NULL;
        if (k == 0) {
            front = rear = data;
            k++;
        }
        else {
            rear->p = data;
            rear = rear->p;
        }
    }
}

// Функция для сжатия файла
void compressFile(ifstream& input, ofstream& output, code* front) {
    unsigned char a = 0;
    int h = 0;  // Счетчик битов в текущем байте (0-7)

    char n;
    while (input.get(n)) {
        code* rear = front;
        // Ищем символ в списке кодов
        while (rear != nullptr && rear->k != n) {
            rear = rear->p;
        }

        if (rear != nullptr && rear->k == n) {
            // Записываем биты кода
            for (int i = 0; i < rear->l; ++i) {
                if (h < 7) {
                    a = (a << 1) | rear->code_arr[i];
                    h++;
                }
                else {
                    a = (a << 1) | rear->code_arr[i];
                    output.put(a);
                    a = 0;
                    h = 0;
                }
            }
        }
    }

    // Дописываем оставшиеся биты
    if (h > 0) {
        a <<= (7 - h);
        output.put(a);
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    //Открытие файла на чтение исходного текста
    ifstream in("sample.txt");

    if (!in.is_open()) {
        cerr << "Не удалось открыть файл для чтения!" << endl;
        return 1;
    }

    //Чтение всего файла в строку
    //C++17 позволяет легко прочитать все, даже спец-символы
    string text((istreambuf_iterator<char>(in)),
        istreambuf_iterator<char>());
    
    cout << text << endl;



    //Открытие файла на запись закодированного текста

    ofstream file("zip.txt");

    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для записи!" << endl;
        return 1;
    }

    in.close();
    file.close();

    return 0;
}