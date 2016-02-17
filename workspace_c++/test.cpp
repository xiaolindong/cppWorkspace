#include <stdio.h>
#include <string.h>

int main(int argc, char* target[]){
    char word[128];
    int result;
    char c;
    int i =0;
    int j;
    int censored =0;
    while  ((c=getchar()) != EOF)
	{
		 if ((c >=65 && c <= 90) || (c >= 97 && c <=122)|| c == 39){
			  word[i] = c;
                i++;}

            for (j=1; j<argc; j++){
                int result=strcmp(word, target[j]);
                if (result == 0 ){
                printf("CENSORED");
                censored =1;
            }
                else if (censored ==0){
                printf("%s", word);
                censored =0;
                }
        else
            printf("%c",'c');
	}

	return 0;
}

