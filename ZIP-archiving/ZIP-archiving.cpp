#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <memory>
#include <bitset>
#include <sys/stat.h> // Для получения размера файла

using namespace std;

long getFileSize(const string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

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
    bool isLeaf() const {
        return (f == nullptr) && (r == nullptr);
    }
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

constexpr int MAX = 256; // Максимальная длина кода

// Глобальные переменные (лучше избегать, но оставим как в оригинале)
std::unique_ptr<Tree> tree;    // Корень дерева
Tree* tree_temp = nullptr;     // Временный указатель для построения дерева
std::unique_ptr<Tree> t;       // Временное хранилище для считанных данных

// Функция для извлечения кодов из файла
void ExtractCodesFromFile(std::ifstream& input) {
    input.read(&t->g, sizeof(char));
    input.read(reinterpret_cast<char*>(&t->len), sizeof(int));
    input.read(reinterpret_cast<char*>(&t->dec), sizeof(int));
}

// Функция преобразования десятичного числа в бинарный массив
void convertDecimalToBinary(int bin[], int decimal, int length) {
    for (int i = length - 1; i >= 0; --i) {
        bin[i] = decimal % 2;
        decimal /= 2;
    }
}

// Функция восстановления дерева Хаффмана
void ReBuildHuffmanTree(ifstream& input, int size) {
    tree = std::make_unique<Tree>();
    tree_temp = tree.get();
    t = std::make_unique<Tree>();

    for (int k = 0; k < size; ++k) {
        tree_temp = tree.get();
        ExtractCodesFromFile(input);

        int bin[MAX] = { 0 };
        int bin_con[MAX] = { 0 };
        convertDecimalToBinary(bin, t->dec, t->len);

        // Копируем только значимые биты
        std::copy(bin, bin + t->len, bin_con);

        // Восстанавливаем путь в дереве
        for (int j = 0; j < t->len; ++j) {
            if (bin_con[j] == 0) {
                if (!tree_temp->f) {
                    tree_temp->f = new Tree();
                    tree_temp->f->f = nullptr;
                    tree_temp->f->r = nullptr;
                }
                tree_temp = tree_temp->f;
            }
            else {
                if (!tree_temp->r) {
                    tree_temp->r = new Tree();
                    tree_temp->r->f = nullptr;
                    tree_temp->r->r = nullptr;
                }
                tree_temp = tree_temp->r;
            }
        }

        // Сохраняем данные в конечном узле
        tree_temp->g = t->g;
        tree_temp->len = t->len;
        tree_temp->dec = t->dec;
    }
}

void decompressFile(ifstream& input, ofstream& output, int originalSize) {
    std::bitset<8> bits;
    Tree* current = tree.get(); // Предполагаем, что tree - корень (уже построен)

    input.read(reinterpret_cast<char*>(&bits), sizeof(char));
    int bitsPos = 0;
    int decodedCount = 0;

    while (decodedCount < originalSize) {
        if (current->isLeaf()) {
            output.put(current->g);
            current = tree.get();
            decodedCount++;
            continue;
        }

        // Получаем следующий бит
        bool bit = bits[7 - bitsPos]; // Биты хранятся старшим вперед
        bitsPos++;

        // Переходим по дереву
        current = bit ? current->r : current->f;

        // Если обработали все 8 бит, читаем следующий байт
        if (bitsPos >= 8) {
            if (!input.read(reinterpret_cast<char*>(&bits), sizeof(char))) {
                break; // Конец файла
            }
            bitsPos = 0;
        }
    }
}


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // 1. Чтение исходного файла
    string inputFilename = "sample2.txt";
    string compressedFilename = "zip.bin";
    string decompressedFilename = "unzipped.txt";
    string codesFilename = "codes.bin";

    ifstream in(inputFilename);
    if (!in.is_open()) {
        cerr << "Не удалось открыть файл для чтения!" << endl;
        return 1;
    }

    string text((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

    if (text.empty()) {
        cerr << "Файл пуст!" << endl;
        return 1;
    }

    // Получаем размер исходного файла
    long originalSize = getFileSize(inputFilename);
    cout << "Размер исходного файла: " << originalSize << " байт" << endl;

    // 2. Подсчет частоты символов
    int freq[256] = { 0 };
    for (char c : text) {
        freq[static_cast<unsigned char>(c)]++;
    }

    // 3. Подготовка уникальных символов и их частот
    char uniqueChars[256];
    int uniqueFreq[256];
    int uniqueCount = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            uniqueChars[uniqueCount] = static_cast<char>(i);
            uniqueFreq[uniqueCount] = freq[i];
            uniqueCount++;
        }
    }

    // 4. Построение дерева Хаффмана
    Min_Stack* minHeap = CreateMin_stack(uniqueChars, uniqueFreq, uniqueCount);
    Node* huffmanTree = CreateHuffmanTree(uniqueChars, uniqueFreq, uniqueCount, minHeap);

    // 5. Запись кодов в промежуточный файл
    ofstream codeFile(codesFilename, ios::binary);
    if (!codeFile.is_open()) {
        cerr << "Не удалось создать файл кодов!" << endl;
        return 1;
    }

    int arr[256];
    printCodeIntoFile(codeFile, huffmanTree, arr);
    codeFile.close();

    // 6. Сжатие исходного файла
    ifstream input(inputFilename);
    ofstream compressed(compressedFilename, ios::binary);
    if (!input || !compressed) {
        cerr << "Ошибка открытия файлов для сжатия!" << endl;
        return 1;
    }

    compressFile(input, compressed, front);
    input.close();
    compressed.close();

    // Получаем размер сжатого файла
    long compressedSize = getFileSize(compressedFilename);
    cout << "Размер сжатого файла: " << compressedSize << " байт" << endl;
    double compressionRatio = (1.0 - static_cast<double>(compressedSize) / originalSize) * 100.0;
    cout << "Коэффициент сжатия: " << compressionRatio << "%" << endl;

    // 7. Распаковка сжатого файла
    ifstream compressedInput(compressedFilename, ios::binary);
    ofstream decompressed(decompressedFilename);
    if (!compressedInput || !decompressed) {
        cerr << "Ошибка открытия файлов для распаковки!" << endl;
        return 1;
    }

    // Восстановление дерева Хаффмана
    ifstream codeInput(codesFilename, ios::binary);
    ReBuildHuffmanTree(codeInput, uniqueCount);
    codeInput.close();

    // Распаковка файла
    decompressFile(compressedInput, decompressed, text.length());
    compressedInput.close();
    decompressed.close();

    // Получаем размер распакованного файла
    long decompressedSize = getFileSize(decompressedFilename);
    cout << "Размер распакованного файла: " << decompressedSize << " байт" << endl;

    // Проверка целостности данных
    if (originalSize == decompressedSize) {
        cout << "Проверка целостности: данные совпадают" << endl;
    }
    else {
        cout << "Проверка целостности: Внимание! Размеры не совпадают" << endl;
    }

    return 0;
}