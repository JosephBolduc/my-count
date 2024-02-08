/* structures used by the first procedure */

struct add_in {
   int left;
   int right;
};
 
struct add_out {
   int value;
};

/* structures used by the first procedure */

struct square_in {
   int value;
};

 
struct square_out {
   int value;
};

/* procedures provided by the server */

program RPCEXAMPLE_PROG {
   version RPCEXAMPLE_VERS {
      add_out ADDPROC( add_in ) = 1;
      square_out SQUAREPROC( square_in ) = 2;
   } = 1;
} = 0x53778894;
