#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <iostream>  
//#include <fstream>  


//using namespace std;


struct sysinfo si;

long total_ram = 0;
long mem_free = 0;
long mem_cached = 0;
//avail_ram = mem_free + mem_cached.
long avail_ram = 0;

void print_proc_meminfo()
{
    printf("------memory info:---------\n");
    printf("Total RAM: %ld KB\n", total_ram/1024);
    printf("Avail RAM: %ld KB\n", avail_ram/1024);
    printf("we are taken RAM:%.2f%%\n", (float)(total_ram - avail_ram)/total_ram*100 );
    printf("---------------------------\n\n");
}
void print_arrow()
{
    printf("   |\n");
    printf("   |\n");
    printf("   |\n");
    printf("   |\n");
    printf("   |\n");
    printf("   |\n");
    printf("  \\|/\n");
}
void get_proc_meminfo()
{
#if 1
    char *file = "/proc/meminfo";
    FILE *fp;
    int nread=0;
    ssize_t len = 0;
    char *buffer = NULL;
    char content[16]="";
    //int mem_free = 0;
    //int mem_cached = 0;

    fp = fopen(file, "rb");
    if(fp == NULL)
    {
        printf("error to open proc/meminfo\n");
        return;
    }

    while(getline(&buffer, &len, fp) != -1) {
      if((strstr(buffer, "MemTotal")) != NULL)
      {
        buffer[strlen(buffer) - 1] = 0;
        sscanf(buffer, "%s%s",content, content);
        total_ram = (long)((strtof(content, NULL))*1024);
      }
      if((strstr(buffer, "MemFree")) != NULL)
      {
        buffer[strlen(buffer) - 1] = 0;
        sscanf(buffer, "%s%s",content, content);
        mem_free = (long)((strtof(content, NULL))*1024);
        printf("\nFree RAM: %ld KB\n", mem_free/1024);
      }
      if((strstr(buffer, "Cached")) != NULL && strstr(buffer, "SwapCached") == NULL)
      {
        buffer[strlen(buffer) - 1] = 0;
        sscanf(buffer, "%s%s",content, content);
        mem_cached = (long)((strtof(content, NULL))*1024);
        printf("\nCached RAM: %ld KB\n", mem_cached/1024);
      }
    }
    
    avail_ram = (long)(mem_free + mem_cached);

    fclose(fp);
 #endif
}

void print_meminfo()
{
    sysinfo(&si);
    printf("Total RAM:    %ld\n", si.totalram);
    printf("buffer RAM:    %ld\n", si.bufferram);
    printf("free RAM:    %ld\n", si.freeram);
    printf("totalswap RAM:    %ld\n", si.totalswap);
    printf("freeswap RAM:    %ld\n", si.freeswap);
    printf("shared RAM:    %ld\n", si.sharedram);
    printf("Available RAM:    %ld\n", si.freeram + si.bufferram);
    printf("Available ratio:    %.2f%%\n", (float)(si.freeram+si.bufferram)/si.totalram*100);
}

float mem_taken_ratio()
{
    //struct sysinfo si;
    //sysinfo(&si);

    return (float)(total_ram - avail_ram)/total_ram*100;
}

void print_help(void)
{
   printf("\n-------------how to use this tool-------------\n");
    printf("eatmem -e 1G            ---this means eat 1GB RAM, change 1G if you want(100M etc.)\n");
    printf("eatmem -p 80%%          ---here -p 80% means taken 80%% of RAM\n");
    printf("eatmem -e 1G -s 100     ---here -s 100 means after 100 seconds will release the RAM, if there is no -s, we will never release the RAM.\n");
    printf("any problem please email me: frankie.lin@yulong.com\n");
    printf("------------------------------------------\n\n");

    printf("\n\n\n-------------history-------------\n");
    printf("version1.2  no -s will never release the RAM.   2017-10-30\n");
    printf("version1.1  int can only save 2GB, change to long. frankie   2017-10-27\n");
    printf("version1.0  init create.  frankie  2017-10-26\n");
    printf("------------------------------------------\n\n");
    //print_meminfo();
    //get_proc_meminfo();
    print_proc_meminfo();
}

int main(int argc, char* argv[])
{
    off64_t kilobytes64 = (off64_t)0;
    
    int cret = 0;  //option flag we get from useer.
    int wait_flag = 0; //user say wait x seconds then release the RAM we are eaten.
    int wait_sec = 0;  //how many seconds we eat the RAM.
    long eat_size = 0; //size of RAM we eat.
    float m_mem_taken_ratio = 20.0; //now the system RAM taken ratio.
    off64_t percent_mem_want2take = (off64_t)50;  //how many percent of RAM we are going to take.
    char *eat_mem = NULL; //point to RAM address we eat.

    get_proc_meminfo();
    //sysinfo(&si);
    //total_ram = si.totalram;

    while((cret = getopt(argc, argv, "e:s:p:")) != EOF) {
        switch(cret) {
//wait x seconds, then the eaten memory will released...
        case 's':
            wait_flag = 1;
            sscanf(optarg,"%d",&wait_sec);
            break;
        case 'e':
            //print_meminfo();
            printf("before eat RAM the memory info:\n");
            get_proc_meminfo();
            print_proc_meminfo();
            print_arrow();

#ifdef NO_PRINT_LLD
            sscanf(optarg,"%ld",&kilobytes64);
#else
            sscanf(optarg,"%lld",(long long *)&kilobytes64);
#endif
            if(optarg[strlen(optarg)-1]=='m' || optarg[strlen(optarg)-1]=='M'){
                printf("\n\neating %ld MB RAM.....\n\n",kilobytes64);

                eat_size = kilobytes64*1024*1024;
            }
            else if(optarg[strlen(optarg)-1]=='g' || optarg[strlen(optarg)-1]=='G'){
                printf("\n\neating %ld GB RAM.....\n\n",kilobytes64);
                eat_size = kilobytes64*1024*1024*1024;
            }
             
            eat_mem = (char*)malloc(eat_size);

            for(long i = 0; i < eat_size; i++)
            {
                *(eat_mem + i) = 0;
            }
          
            print_arrow();
            printf("\nafter eating, the RAM state:\n");
            //print_meminfo();
            get_proc_meminfo();
            print_proc_meminfo();
        break;

//how many memory do you want to take. etc 80%
//you should enter: eatmem -t 80%
        case 'p':
            //print_meminfo();
            printf("before eat RAM the memory info:\n");
            get_proc_meminfo();
            print_proc_meminfo();
            print_arrow();

#ifdef NO_PRINT_LLD
            sscanf(optarg,"%ld",&percent_mem_want2take );
#else
            sscanf(optarg,"%lld",(long long *)&percent_mem_want2take );
#endif
            if(optarg[strlen(optarg)-1]=='%'){
                printf("we want to taken %ld%% RAM.....\n",percent_mem_want2take );

                m_mem_taken_ratio = mem_taken_ratio();
                if(percent_mem_want2take > m_mem_taken_ratio) {
                    eat_size = ((percent_mem_want2take - m_mem_taken_ratio)/100)*total_ram;
                    printf("\n yes... we are going to eat : %ld B...\n\n\n", eat_size);
            
                    eat_mem = (char*)malloc(eat_size);

                    for(long i = 0; i < eat_size; i++)
                    {
                        *(eat_mem + i) = 0;
                    }

                    print_arrow();
                    printf("after eating the RAM state:\n");
                    //print_meminfo();
                    get_proc_meminfo();
                    print_proc_meminfo();
                }
                else {
                    printf("no memory to eat!!!\n\n\n");
                 }
            }
           break;

        default:
           print_help();
           break;
        }
        
    }

    if(eat_mem != NULL) {
        
        print_arrow();
        //release the RAM we eat.
        if(wait_flag == 1)
        {
            printf("\nwaiting for release the RAM...........\n\n\n");
            for(int i = 0; i< wait_sec;i++) {
                printf("...%d sec...\n", i);
                sleep(1);
            }
            printf("\n");
            wait_flag = 0;
        }
        else {
            printf("\nno -s, so never release the RAM..........\n\n\n");
            //for(int i = 0; i < 20; i++) {
            //never stop
            for(;;) {
                printf("\nno -s, so never release the RAM..........\n\n\n");
                sleep(1);
            }
        }

        free(eat_mem);
        eat_mem = NULL;

        print_arrow();
        printf("memory releaseddddddddd!\n");
        //print_meminfo();
        //system("cat /proc/meminfo > /dev/null");
        //get_proc_meminfo();
        //print_proc_meminfo();
    }

    if(argc == 1) print_help();
    //if(argc == 1) print_proc_meminfo();
}
