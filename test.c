// Un exemple de programme MiniC
int start = 0;
int end = 5;
void main () {
	int i, s = start , e = end ;
	int sum = 0;
	sum = start;
	for (i = s; i < e; i = i + 2) {
 		sum = sum + 1;	
 		sum = sum +2;

 	}
 	while(sum < start){
 		sum = ~start;
 	}
	print("sum :", sum , "\n") ;
}