#include <stdio.h>
#include <string.h>

#define FRAMESIZE (1<<8)
#define TLBSIZE 16

const int mask = (1 << 8) - 1;
int len_page = 0;
int len_frame = 0;
int len_tlb = 0;

FILE *bStore;

struct page{
    int offset;
    int page_number;
    int frame;
};

struct page pages[FRAMESIZE];
struct page tlb[TLBSIZE];
char ram[FRAMESIZE][FRAMESIZE];

int get_frame(struct page* curr){
    char f = 0;

    for(int i = len_tlb; i >= 0; i--){
        if(tlb[i].page_number == curr->page_number){
            curr->frame = tlb[i].frame;
            f = 1;
            break;
        }
    }

    for(int i = 0; i < len_page && !f; i++){
        if(pages[i].page_number == curr->page_number){
            curr->frame = pages[i].frame;
            f = 1;
            break;
        }
    }
    if(!f){
        fseek(bStore, curr->page_number * FRAMESIZE, SEEK_SET);
        fread(ram[len_frame], sizeof(char), FRAMESIZE, bStore);

        pages[len_page].page_number = curr->page_number;
        pages[len_page].frame = len_frame;
        len_page++;

        curr->frame = len_frame++;
    }

    if(len_tlb == TLBSIZE){
        memmove(&tlb[0], &tlb[1], sizeof(struct page) * 15);
        len_tlb--;
    }
    tlb[len_tlb++] = *curr;

    return 0;
}

struct page getPageAndOffset(int address){
    struct page curr_n;
    curr_n.offset = address & mask;
    curr_n.page_number = (address >> 8) & mask;
    return curr_n;
}

int main(int argc, char* argv[]){
    if(argc != 2 || !freopen(argv[1],"r",stdin)){
        printf("error read file");
        return 1;
    }
    bStore = fopen("./BACKING_STORE.bin","rb");
    if(bStore == NULL){
        printf("error bStore read");
        return 1;
    }
    freopen("check.txt","w",stdout);

    int inAddress = 0;
    while(scanf("%d",&inAddress) == 1){
        struct page res = getPageAndOffset(inAddress);
        get_frame(&res);
        printf("Virtual address: %d Physical address: %d Value: %d\n",
               (res.page_number<<8) | res.offset,
               (res.frame << 8) | res.offset,
               ram[res.frame][res.offset]);
    }
}