#!/bin/sh


  
IS_RECURSIVE=0
DEF_CASE=0



# sed pattern for uppercase 's/\(.*\)/\U\1/'
# sed pattern for lowercase 's/\(.*\)/\L\1/'

SED_PATTERN=""
SCRIPT_NAME=`basename $0`
 
ShowHelp(){

	echo "modify [-r] [-l|-u] <dir/filenames...>"
	echo "modify [-r] <sed pattern> <dir/filenames...>"
	echo "modify [-h]"
	echo "r---use recursion\tl---lowercase the filename\tu---uppercase the filename\th---showhelp(which shows this message)"
	echo "\nCorrect examples:\n\t./script_filename.sh -u 'test_file.txt'\n" 
	echo "\t./script_filename.sh -l 'TEST_FILE.txt'\n"
	echo "\t./script_filename.sh -r -u 'full/directory/from/home/test_folder'\n"
	echo "\t./script_filename.sh -h\n"
	echo "\t./script_filename.sh -r 's/\(.*\)/\U\1/' 'full/directory/from/home/test_folder'\n"
	echo "\t./script_filename.sh 's/\(.*\)/\L\1/' 'full/directory/from/home/test_folder/TEST_FILE.txt'\n"
	
}
 
ModifyFile(){

	TEMP_FILE=`basename "$1"`
    TEMP_PATH=`dirname "$1"`
    FILE_NAME=${TEMP_FILE%.*}
    FILE_EXT=${TEMP_FILE##*.}
    IS_A_FILE=0 
		 		
 
                
    if [ "$TEMP_FILE" = "$SCRIPT_NAME" ] ; then
	    return
    fi
      
    if [ -f "$TEMP_PATH/$TEMP_FILE" ];then
    	IS_A_FILE=1
    fi        
                
                
    if [ "$TEMP_FILE" = "$FILE_NAME.$FILE_EXT" -a $IS_A_FILE -eq 1  ];then
                
	    case $DEF_CASE in
 	      "1") UPDATED=`echo "$FILE_NAME" |  tr 'A-Z' 'a-z'`.$FILE_EXT ;;
	      "2") UPDATED=`echo "$FILE_NAME" |  tr 'a-z' 'A-Z'`.$FILE_EXT ;;
	      "3") UPDATED=`echo "$FILE_NAME" |  sed -s $SED_PATTERN`.$FILE_EXT ;;
	    esac
	    
		
	elif [ ! "$TEMP_FILE" = "$FILE_NAME.$FILE_EXT" -a $IS_A_FILE -eq 1 ];then
		
		case $DEF_CASE in
	   		"1") UPDATED=`echo "$TEMP_FILE" |  tr [:upper:] [:lower:]` ;;
		   	"2") UPDATED=`echo "$TEMP_FILE" |  tr [:lower:] [:upper:]` ;;
		    "3") UPDATED=`echo "$TEMP_FILE" |  sed -s $SED_PATTERN` ;;
	   	esac
	   	
	else
	
		return
		
	fi
	
	if [ -f "$TEMP_PATH/$UPDATED" ] ; then
    	`echo "File '$UPDATED' already exists in the same directory '$TEMP_PATH/$UPDATED'\n" 1>&2`
	    return
    else
	    mv "$1" "$TEMP_PATH/$UPDATED"
   	fi
		    
}



CheckFile(){
        #check if the file exists in the directory
        if [ ! -f "$1" -a ! -d "$1" ] ; then
        	`echo "\nPlease check the input filename or the directory again. It does not exists.\n" 1>&2`
            return
        fi
 
        if [ ! -w "$1" -a -f "$1" ]; then
        	`echo "\nFile '$1' cannot be modified please change the permissions to this file.\n" 1>&2`
            return
        fi
 
        if [ $IS_RECURSIVE -eq 1 ] ; then
        	if [ -d "$1" ] ; then

                find "$1"  -depth  | while read -r file ; do
                
        	        ModifyFile "$file"
                done

                else    
                
        	        ModifyFile "$1"
                fi
        else     
              
        	ModifyFile "$1"
        fi
}
 
if [ $# -eq 0 ]; then
        ShowHelp
fi
 
while [ "$1" != "" ]; do
	case "$1" in
	    -r)  IS_RECURSIVE=1 ;;
        -l)  DEF_CASE=1 ;;
	    -u)  DEF_CASE=2 ;;
        -h)  ShowHelp
                exit ;;
         *)
    	    
            if [ $DEF_CASE -eq 0 ]; then
            	DEF_CASE=3
                SED_PATTERN="$1"
            else
                CheckFile "$1"
            fi
            ;;
	esac
	
shift
done
