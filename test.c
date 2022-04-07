// Un exemple de programme MiniC
int start = 0;
int end = 100;
void main () {
	int i, s = start , e = end ;
	int sum = 0;
	for (i = s; i < e; i = i + 1) {
		sum = sum + i;		
 	}
 	//sum = sum;
 	print ("sum: ", sum , "\n") ;
}