**Dobrica Nicoleta Adriana**
**311CA**

## The Library of HashBabel

### Description:

* We have a library that contains different books, and users who can do certain actions with those books. These said users, library and books were all implemented with the use of generic hashtables. In this library we can add books, we can get information about them (e.g their rating or how many people have purchased them), we can delete a book etc. Also, users can either loan a book, return it or lose it.

### ADD_BOOK:
* This function adds a book into our main hashtable (the library), along with a given number of definitions. For the sake of simplicity, the book in itself is a structure which contains a generic hashtable (the actual book), and information about it (its rating, name, whether it is currently borrowed or not etc).

### GET_BOOK:
* This function gets information about a desired book and prints it in a certain format. The information required is: the book's name, its rating and its number of purchases.

### REMOVE_BOOK:
* This function removes a given book from the library, and all its information. 

### ADD_USER:
* When used, this function adds a new user into the system. The actual user is also a structure which contains the user's name, points, whether the user has been banned or not or if the user is currently borrowing a book or not. Every user at the beginning has 100 points, and can either lose or gain more points depending on their actions (if they return a book before their number of available days is up or if they lose a book).

### BORROW_BOOK:
* This is one of the things a user can do in the library: borrow a book. Alongside the name of the book, the user has a number of days in which they have to read the book and return it. If the user is banned they are not allowed to borrow a book anymore, or if the user is already borrowing a book they cannot borrow another.

### RETURN_BOOK:
* After reading the borrowed book, a user can return it, along with the number of days since the book has been borrowed and the rating of the book. Depending on the number of days since the book has been borrowed, the user can either gain or lose more points. If the total number of points is negative, then the user will sadly be banned from the library. 
* After the rating of the book is given, the information on the returned book will also be updated: the number of purchases will increase, and the rating of book will be updated.

### LOST_BOOK:
* If the reader borrows a book and then loses it, the said user will lose 50 points and the book will be removed from the library.

### ADD_DEF:
* This function adds a new definition (a pair of a key and a value) to an already existing book. If the book doesn't exist, then an error message will pop up.

### GET_DEF:
* This function prints a certain definition from a given book based on its key. If either the book or the key doesn't exist, an error message will pop up. 

### RMV_DEF:
* This function gets a key and removes a certain definition from a book based on the key. 

### EXIT:
* When this command is given, all the used hashtables will be freed from memory, and a ranking of the top users and top books will be printed. The ranking of the books was done using qsort, and the ranking of the users was done with bubble sort.

### RESIZE:
* This function will resize a hashtable, the new size being the double of the hashtable's hashmax (which is 10 at the beginning). A new hashtable will be created. We traverse the old hashtable, then each of the buckets, putting the data from the old hashtable into our new hashtable, and freeing the current node and data from memory.

### Additional info:

* I think I could have done a better function for resizing the hashtable.
* This homework helped me grasp a better understanding on how hashtables actually work and how to manipulate them and the different data they store.
