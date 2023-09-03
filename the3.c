#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "the3.h"


/*implementation of DomainFreqList*/
struct DomainFreqNode* mk_freqnode() {
    struct DomainFreqNode *freq_node;
    freq_node = (struct DomainFreqNode *) malloc(sizeof(struct DomainFreqNode));
    if (freq_node) {
        freq_node -> name = NULL;
        freq_node -> freq = 0;
        freq_node -> prev = NULL;
        freq_node -> next = NULL;
    }
    return freq_node;
}

struct DomainFreqNode* insert_freqnode(struct DomainFreqNode **list_pointer,char *name, int freq) {
    struct DomainFreqNode *freq_node = mk_freqnode();
    struct DomainFreqNode *curr,*prev;
    if(freq_node) {
        freq_node -> name = (char *)malloc((strlen(name)+1)*sizeof(char));
        strcpy(freq_node -> name, name);
        freq_node -> name[strlen(name)] = '\0';
        freq_node -> freq = freq;
        
        curr = *list_pointer;
        prev = NULL;  
        while(curr&&(freq < curr->freq || (freq == curr -> freq && strcmp(name,curr->name)>0))) {
            prev = curr;
            curr = curr -> next;
        } 
        freq_node -> next = curr;
        freq_node -> prev = prev;
        if(prev) {
            prev -> next = freq_node;
        }
        else {
            *list_pointer = freq_node;
        }
        if(curr) {
            curr -> prev = freq_node;
        }
    return *list_pointer;
    }
}

struct CacheNode* mk_cachenode() {
    struct CacheNode *cache_node;
    cache_node = (struct CacheNode *) malloc(sizeof(struct CacheNode));
    if(cache_node) {
        cache_node -> media.name = NULL;
        cache_node -> media.size = 0;
        cache_node -> prev = NULL;
        cache_node -> next = NULL;
        cache_node -> domainFreqList = NULL;
        
        cache_node->domainFreqList = (struct DomainFreqList *) malloc(sizeof(struct DomainFreqList));
        if (cache_node->domainFreqList) {
            cache_node->domainFreqList->head = NULL;
            cache_node->domainFreqList->tail = NULL;
        } 
        
    }
    return cache_node;
}

struct CacheNode* insert_cachenode(struct Cache *cache,int z) {
    struct CacheNode *cache_node = mk_cachenode();
    if (z!=0) {
        cache -> tail -> next = cache_node;
        cache_node -> prev = cache -> tail;
        cache_node -> next = NULL;
        cache -> tail = cache_node;
    }
    else {
        cache -> head = cache_node;
        cache -> tail = cache_node;
        cache_node -> next = NULL;
        cache_node -> prev = NULL;
    }
    return cache_node;
}
/*finally, implementation of cache*/
Cache* createCache()
{
    char Line[1000];
    int i,tokenSize,tokenNum,token_freq;
    char *token,*token_name,*token_second;
    struct Cache *cache;
    struct CacheNode *node;

    cache = (struct Cache *) malloc(sizeof(struct Cache));
    
    if(cache) {
        scanf("%i %i\n",&cache->cacheLimit,&cache->mediaCount);
        cache->head = NULL;
        cache -> tail = NULL;
        cache -> currentSize = 0;
        
        for(i=0;i<cache->mediaCount;i++) {
            node = insert_cachenode(cache,i);
            fgets(Line,1000,stdin);

            token = strtok(Line," ");
            tokenSize = strlen(token);

            node -> media.name = (char *) malloc(sizeof(char)*(tokenSize +1));
            strcpy(node->media.name,token);
            node->media.name[tokenSize] = '\0';

            token = strtok(NULL, " ");
            tokenNum = atoi(token);
            node -> media.size = tokenNum;
            cache->currentSize += node -> media.size;
            
            while(token_name=strtok(NULL," ")) { 
                
                token_second = strtok(NULL," ");
                token_freq = atoi(token_second);
                insert_freqnode((&node->domainFreqList->head),token_name,token_freq);
            }
        }
    }
    return cache;
}

void printDomFreq(struct DomainFreqList *freqlist) {
    struct DomainFreqNode *freqnode;
    freqnode = freqlist -> head; 
    printf("\t------- Media Domain Frequencies -------\n");
    while(freqnode) {
        printf("\t\tName: %s\t\tFreq: %i\n",freqnode -> name,freqnode->freq);
        freqnode = freqnode -> next;
    }
    printf("----------------------------------------\n");
}

void printCache(Cache* cache)
{
    struct DomainFreqNode *freqnode;
    struct DomainFreqList *freqlist;
    struct CacheNode *cache_node;
    int i;
    cache_node = cache->head;
    if (cache_node) {
        printf("-------------- Cache Information --------------\n");
        printf("Cache Limit: %i KB\n",cache->cacheLimit);
        printf("Current Size: %i KB\n",cache->currentSize);
        printf("Media Count: %i\n",cache->mediaCount);
        printf("Cache Media Info:\n");
        for(i=0;i<(cache->mediaCount);i++) {
            freqlist = cache_node -> domainFreqList;
            freqnode = freqlist -> head; 
            printf("\tName: %s\tSize: %i KB\n",(cache_node->media).name,(cache_node->media).size);
            printf("\t------- Media Domain Frequencies -------\n");
            while(freqnode) {
                printf("\t\tName: %s\t\tFreq: %i\n",freqnode -> name,freqnode->freq);
                freqnode = freqnode -> next;
            }
            printf("\t\t----------------------------------------\n");
            cache_node = cache_node->next;
            }
        printf("----------------------------------------------\n");
        }
    else {
        printf("Cache is empty\n");
    }
}

/*
    Finds the corresponding media's node in the cache
    Return NULL if it is not found
*/
CacheNode* findMedia(Cache* cache, char* name)
{
    struct CacheNode *ptr_cachenode;
    ptr_cachenode = cache -> head;
    while (ptr_cachenode) {
        if (!strcmp(ptr_cachenode -> media.name,name)) {
            break;
        }
        else {
            ptr_cachenode = ptr_cachenode -> next;
        }
    }
    if(ptr_cachenode) {
        return ptr_cachenode;
    }
    else {
        return NULL;
    }
}
/*
    Adds media to the cache
*/
CacheNode* addMediaRequest(Cache* cache, Media media, char* domain)
{
    struct CacheNode *ptr_cachenode,*prev_cache,*next_cache,*new_cachenode;
    struct DomainFreqNode *ptr_domfreqnode,*prev_freq=NULL,*next_freq,*curr,*prev;
    int freq=0;
    char *name=NULL;
    /*if the media exists*/
    if(ptr_cachenode = findMedia(cache,media.name)) {
        prev_cache = ptr_cachenode -> prev;
        next_cache = ptr_cachenode -> next;
        
        if(prev_cache) {
            prev_cache -> next = next_cache;
            next_cache -> prev = prev_cache;
            ptr_cachenode -> next = cache -> head;
            (cache -> head) -> prev = ptr_cachenode;
            cache -> head = ptr_cachenode;
        }
        /*manage the frequency nodes*/
        ptr_domfreqnode = (ptr_cachenode -> domainFreqList) -> head;
        next_freq = ptr_domfreqnode -> next; 
        
        while (ptr_domfreqnode) {
            if(strcmp(ptr_domfreqnode -> name, domain)==0) {
                break;
            }
            else {
                prev_freq = ptr_domfreqnode;
                next_freq = (ptr_domfreqnode->next) ? ptr_domfreqnode->next->next : NULL;
                ptr_domfreqnode = ptr_domfreqnode->next;
            }
            
            /*this loop returns a pointer to the domain freq node if present and otherwise returns NULL*/
        }
        /*if the frequency node exists*/
        if(ptr_domfreqnode) {
            (ptr_domfreqnode -> freq)++; 
            
            ptr_domfreqnode -> prev = NULL;
            ptr_domfreqnode -> next = NULL;
            
            if(prev_freq) {
                prev_freq -> next = next_freq;
            }
            if(next_freq) {
                next_freq -> prev = prev_freq;
            }
            
            curr = (ptr_cachenode -> domainFreqList) -> head;
            prev = NULL;  
            freq = ptr_domfreqnode -> freq;
            name = ptr_domfreqnode -> name;
            
            while(curr&&(freq < curr->freq || (freq == curr -> freq && strcmp(name,curr->name)>0))) {
                prev = curr;
                curr = curr -> next;
            } 
            
            ptr_domfreqnode -> next = curr;
            ptr_domfreqnode -> prev = prev;
            
            if(prev) {
                prev -> next = ptr_domfreqnode;
            }
            else {
                (ptr_cachenode -> domainFreqList) -> head = ptr_domfreqnode;
            }
            if(curr) {
                curr -> prev = ptr_domfreqnode;
            }
        }
        
        /*if the frequency node doesn't exist*/
        else {
            insert_freqnode(&((ptr_cachenode->domainFreqList)->head),domain,1);
        }
    }
    
    else {
        new_cachenode = mk_cachenode();
        if(new_cachenode) {
            new_cachenode -> next = cache -> head;
            new_cachenode -> prev = NULL;
            
            if(cache -> head) {
                (cache -> head) -> prev = new_cachenode;
            }
            
            cache -> head = new_cachenode;
            new_cachenode -> media = media;
            insert_freqnode(&((new_cachenode->domainFreqList)->head),domain,1);
        }
        if((cache -> currentSize + (new_cachenode -> media).size) <= (cache -> cacheLimit)) {
            cache -> currentSize += (new_cachenode -> media).size;
            (cache -> mediaCount)++;
        }
        else {
            while ((cache -> currentSize + (new_cachenode -> media).size) > (cache -> cacheLimit)) {
                (cache -> currentSize) -= (cache -> tail)->media.size;
                (cache -> mediaCount)--;
                
                cache -> tail = cache -> tail -> prev;
                cache -> tail -> next -> prev = NULL;
                free(cache -> tail -> next -> media.name);
                free(cache -> tail -> next -> domainFreqList);
                free(cache -> tail -> next);
                cache -> tail -> next = NULL;
                
                
            }
            (cache -> currentSize) += (new_cachenode -> media).size;
            (cache -> mediaCount)++;
        }
    }
    return ptr_cachenode;
}

/*
    Finds a media from its name and deletes it's node from the cache
    Do not do anything if media is not found
*/
void deleteMedia(Cache* cache, char* name)
{
    struct CacheNode *ptr_cachenode,*ptr_prev,*ptr_next;
    ptr_cachenode = findMedia(cache,name);
    if(ptr_cachenode) {
        ptr_prev = ptr_cachenode -> prev;
        ptr_next = ptr_cachenode -> next;
        
        ptr_prev -> next = ptr_next;
        ptr_next -> prev = ptr_prev;
        
        cache -> currentSize -= (ptr_cachenode -> media).size;
        cache -> mediaCount--;
        
        free((ptr_cachenode -> media).name);
        free(ptr_cachenode -> domainFreqList);
        free(ptr_cachenode);
    }
    
}

