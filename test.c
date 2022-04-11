// Un exemple de programme MiniC
int start = 0;
int end = 100;
bool x, y = true;
void main () {
	int i, s = start , e = end ;
	int sum = 0;
	for (i = s; i < e; i = i + 1) {
		sum = sum + i;		
 	}
 	if ( sum == 1){
 		x = y || true ;
 	}
	print ("sum: ", sum , "\n") ;
}