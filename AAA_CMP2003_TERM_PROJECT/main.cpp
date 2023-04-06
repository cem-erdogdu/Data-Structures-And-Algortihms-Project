#include <iostream>
#include <string.h>
#include <fstream>
#include <chrono>
#include "binarysearchtree.h"
#include "binarytree.h"
 
# define MAX_CHARS 128
# define MAX_WORD_SIZE 10000

using namespace std;
using namespace std::chrono;
 
// A Trie node
struct TrieNode
{
    bool isEnd; // indicates end of word
    unsigned int frequency;  // the number of occurrences of a word
    int indexMinHeap; // the index of the word in minHeap
    TrieNode* child[MAX_CHARS]; // represents 26 slots each for 'a' to 'z'.
};
 
// A Min Heap node
struct MinHeapNode
{
    TrieNode* root; // indicates the leaf node of TRIE
    unsigned int frequency; //  number of occurrences
    char* word; // the actual word stored
};
 
// A Min Heap
struct MinHeap
{
    unsigned capacity; // the total size a min heap
    int count; // indicates the number of slots filled.
    MinHeapNode* array; //  represents the collection of minHeapNodes
};
 
// A utility function to create a new Trie node
TrieNode* newTrieNode()
{
    // Allocate memory for Trie Node
    TrieNode* trieNode = new TrieNode;
 
    // Initialize values for new node
    trieNode->isEnd = 0;
    trieNode->frequency = 0;
    trieNode->indexMinHeap = -1;
    for( int i = 0; i < MAX_CHARS; ++i )
        trieNode->child[i] = NULL;
 
    return trieNode;
}
 
// A utility function to create a Min Heap of given capacity
MinHeap* createMinHeap( int capacity )
{
    MinHeap* minHeap = new MinHeap;
 
    minHeap->capacity = capacity;
    minHeap->count  = 0;
 
    // Allocate memory for array of min heap nodes
    minHeap->array = new MinHeapNode [ minHeap->capacity ];
 
    return minHeap;
}
 
// A utility function to swap two min heap nodes. This function
// is needed in minHeapify
void swapMinHeapNodes ( MinHeapNode* a, MinHeapNode* b )
{
    MinHeapNode temp = *a;
    *a = *b;
    *b = temp;
}
 
// This is the standard minHeapify function. It does one thing extra.
// It updates the minHeapIndex in Trie when two nodes are swapped in
// in min heap
void minHeapify( MinHeap* minHeap, int idx )
{
    int left, right, smallest;
 
    left = 2 * idx + 1; // left child
    right = 2 * idx + 2; // right child
    smallest = idx;
    if ( left < minHeap->count &&
         minHeap->array[ left ]. frequency <
         minHeap->array[ smallest ]. frequency
       )
        smallest = left;
 
    if ( right < minHeap->count &&
         minHeap->array[ right ]. frequency <
         minHeap->array[ smallest ]. frequency
       )
        smallest = right;
 
    if( smallest != idx )
    {
        // Update the corresponding index in Trie node.
        minHeap->array[ smallest ]. root->indexMinHeap = idx;
        minHeap->array[ idx ]. root->indexMinHeap = smallest;
 
        // Swap nodes in min heap
        swapMinHeapNodes (&minHeap->array[ smallest ], &minHeap->array[ idx ]);
 
        minHeapify( minHeap, smallest );
    }
}
 
// A standard function to build a heap
void buildMinHeap( MinHeap* minHeap )
{
    int n, i;
    n = minHeap->count - 1;
 
    for( i = ( n - 1 ) / 2; i >= 0; --i )
        minHeapify( minHeap, i );
}
 
// Inserts a word to heap, the function handles the 3 cases explained above
void insertInMinHeap( MinHeap* minHeap, TrieNode** root, const char* word )
{
    // Case 1: the word is already present in minHeap
    if( (*root)->indexMinHeap != -1 )
    {
        ++( minHeap->array[ (*root)->indexMinHeap ]. frequency );
 
        // percolate down
        minHeapify( minHeap, (*root)->indexMinHeap );
    }
 
    // Case 2: Word is not present and heap is not full
    else if( (minHeap->count) < (minHeap->capacity) )
    {
        int count = minHeap->count;
        minHeap->array[ count ]. frequency = (*root)->frequency;
        minHeap->array[ count ]. word = new char [strlen( word ) + 1];
        strcpy( minHeap->array[ count ]. word, word );
 
        minHeap->array[ count ]. root = *root;
        (*root)->indexMinHeap = minHeap->count;
 
        ++( minHeap->count );
        buildMinHeap( minHeap );
    }
 
    // Case 3: Word is not present and heap is full. And frequency of word
    // is more than root. The root is the least frequent word in heap,
    // replace root with new word
    else if ( (*root)->frequency > minHeap->array[0]. frequency )
    {
 
        minHeap->array[ 0 ]. root->indexMinHeap = -1;
        minHeap->array[ 0 ]. root = *root;
        minHeap->array[ 0 ]. root->indexMinHeap = 0;
        minHeap->array[ 0 ]. frequency = (*root)->frequency;
 
        // delete previously allocated memory and
        delete [] minHeap->array[ 0 ]. word;
        minHeap->array[ 0 ]. word = new char [strlen( word ) + 1];
        strcpy( minHeap->array[ 0 ]. word, word );
 
        minHeapify ( minHeap, 0 );
    }
}
 
// Inserts a new word to both Trie and Heap
void insertUtil ( TrieNode** root, MinHeap* minHeap,
                  const char* word, const char* dupWord )
{
    // Base Case
    if ( *root == NULL )
        *root = newTrieNode();

    //  There are still more characters in word
    if ( *word != '\0' )
        insertUtil ( &((*root)->child[ *word - 1 ]),
                     minHeap, word + 1, dupWord );
    else // The complete word is processed
    {
        // word is already present, increase the frequency
        if ( (*root)->isEnd )
            ++( (*root)->frequency );
        else
        {
            (*root)->isEnd = 1;
            (*root)->frequency = 1;
        }

        // Insert in min heap also
        insertInMinHeap( minHeap, root, dupWord );
    }
}
 
 
// add a word to Trie & min heap.  A wrapper over the insertUtil
void insertTrieAndHeap(const char *word, TrieNode** root, MinHeap* minHeap)
{
    insertUtil( root, minHeap, word, word );
}
 
// A utility function to show results, The min heap
// contains k most frequent words so far, at any time
void displayMinHeap( MinHeap* minHeap )
{
    int i;
 
    // print top K word with frequency
    for( i = 0; i < minHeap->count; ++i )
    {
        cout<<minHeap->array[i].word<<"  :  "<<minHeap->array[i].frequency<<"\n";
        
    }
}
 



void Index(ofstream& outputfile, const string& paragraph, const string unigram, const string& creator,bSearchTreeType<string>& binary)
{   
    string number_nex_to_string;// bu unigramcounttaki kelimelerin yanında yer alan sayı
    int doubleQuoteStart, numberStart;
    int aStart;
    bool flag3 = false, flag2 = false, flag1 = false, isInvalid = false;
    string unigramWord="";
    int unigramCount=0;

    MinHeap* minHeap = createMinHeap(10);

     
    // Create an empty Trie
    TrieNode* root = NULL;


    for (int i = 0; i < paragraph.length(); i++) { // i am looping through the paragraph's length and
        //cout<<"1"<<endl;
        if (flag2 == false) {
            if (paragraph.substr(i, unigram.length()) == unigram) { // Copies the substring starting from ith position to length of the substring
                flag2 = true;
                aStart = i;

                i += 14;
                continue;
            }
        }
        else if (flag2 == true) {
            if (paragraph.substr(i, creator.length()) == creator) {
                flag2 = false;
                //cout << paragraph[i] << paragraph[i+1] << endl;
                //string str = paragraph.substr(aStart, i - aStart - 1);
               // if(!once){
                    //once = true;
                    //string str = paragraph.substr(aStart, i - aStart - 1);
                    //cout << aStart << endl << "-" << str << endl << endl << endl;
                //}

                //i-=4;
            }
            else {
                string a = "\":", b = "\"";
                if (paragraph.substr(i, 2) == "\":") {
                    if (flag1 && (int(paragraph.substr(i + 2, 1)[0]) >= 48 && int(paragraph.substr(i + 2, 1)[0]) <= 57)) {
                        flag1 = false;
                        //cout << aStart << endl;  // double quote " adresini tuttuğu için bir arttırdı othersın o suna geldi
                        // bu kısım "safas" arasındaki kelimeyi alıyor stringe
                        unigramWord = paragraph.substr(doubleQuoteStart + 1, i - doubleQuoteStart - 1);

                        int quatationMark = 0;
                        bool isCharacter = false;
                        for(int c = 0; c < unigramWord.length();c++){
                            char a = unigramWord[c];
                            if((int(a) >= 97 && int(a) <= 122)){
                                isCharacter = true;
                            } else { 
                                if(c>0 && c<unigramWord.length()){
                                    quatationMark+=1;
                                    if(quatationMark==2){
                                        isInvalid = true;
                                        break;
                                    }
                                }   
                            }
                            unigramWord[c]=tolower(a);
                        }

                        if(!isCharacter){
                            isInvalid = true;
                        }
                        //  s.erase(s.length()-1,1);
                        int first, last;
                        if(unigramWord.length()>1){
                            first = int(unigramWord[0]);

                            if((first < 97 || first > 122)){
                                unigramWord.erase(0,1);
                            }

                            last = int(unigramWord[unigramWord.length()-1]);
                        
                            if((last < 97 || last > 122)){
                            unigramWord.erase(unigramWord.length()-1,1);
                            }
                        }   //c-=1;
                                     
                        outputfile << unigramWord << "\n";
                        //cout << str << endl;
                        //cout << i << " --- " << endl;
                        i += 1;



                        numberStart = i;
                        ////outputfile << unigramWord << " ";
                        //outputfile << str << " - ";
                        //cout << str << " - " << endl;
                    }
                }
                else if (paragraph.substr(i, 1) == "\"") {
                    if (!flag1) {
                        doubleQuoteStart = i;
                        flag1 = true;
                        isInvalid = false;
                        //cout << i << endl;
                    }
                }
                else {
                    if (flag1 == false && !isInvalid) {
                        if (paragraph.substr(i, 1) == ",") { // burada direk sayıyı alıyoruz
                        
                            unigramCount = stoi(paragraph.substr(numberStart + 1, i - numberStart - 1)); // burada bu variable integer olarak tanımlandığında, ascii
                            string str = paragraph.substr(numberStart + 1, i - numberStart - 1);
                            if(!binary.search(unigramWord)){
                                char * buff = &unigramWord[0];
                                for(int i = 0;i<unigramCount;i++){
                                // cout << buff << ""<<"\n";
                                    insertTrieAndHeap(buff, &root, minHeap);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    displayMinHeap( minHeap );
}




int main() {

    bSearchTreeType<string> binary; // binary search tree object


    auto begin = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
    begin = std::chrono::high_resolution_clock::now();


    string word;   
    ifstream infilestop;
    infilestop.open("stopwords.txt");

    while (getline(infilestop, word)) {
        binary.insert(word);
        
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    int length = 0;
    int start = 0;
    string paragraph;
    string unigrams = "unigramCount\""; 
    string creator = "{\"creator\""; 
    ifstream infile;
    ofstream outfile;
    outfile.open("Indexes.txt");
    infile.open("PublicationsDataSet.txt");
   
    while (getline(infile, word)) {
        paragraph += word;
    }
    length = paragraph.length();
  
    

    begin = std::chrono::high_resolution_clock::now();
    Index(outfile, paragraph, unigrams, creator,binary);
    outfile.close();
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);

}
