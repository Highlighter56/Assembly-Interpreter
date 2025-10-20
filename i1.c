// i1shell.c
// Patrick Hughes
#include <stdio.h>    // for I/O functions
#include <stdlib.h>   // for exit()
#include <time.h>     // for time functions

FILE *infile;
short r[8], mem[65536], offset6, imm5, imm9, pcoffset9, pcoffset11, 
      regsave1, regsave2;
unsigned short ir, pc, opcode, code, dr, sr, sr1, sr2, baser, bit5, bit11,
               trapvec, eopcode, n, z, c, v;
char letter;

void setnz(short r)
{
   n = z = 0;
   if (r < 0)    // is result negative?
      n = 1;     // set n flag
   else
   if (r == 0)   // is result zero?
      z = 1;     // set z flag
}

void setcv(short sum, short x, short y)
{
   v = c = 0;
   if (x >= 0 && y >= 0)   // if both non-negative, then no carry
      c = 0;
   else
   if (x < 0 && y < 0)     // if both negative, then carry
      c = 1;
   else
   if (sum >= 0)           // if signs differ and sum non-neg, then carry
      c = 1;
   else                    // if signs differ and sum neg, then no carry
      c = 0;
   // if signs differ then no overflow
   if ((x < 0 && y >= 0) || (x >= 0 && y < 0))
      v = 0;
   else
   // if signs the same and sum has different sign, then overflow
   if ((sum < 0 && x >= 0) || (sum >= 0 && x < 0))
      v = 1;
   else
      v = 0;
}

int main(int argc, char *argv[])
{
   time_t timer;

   if (argc != 2)
   {
       printf("Wrong number of command line arguments\n");
       printf("Usage: i1 <input filename>\n");
       exit(1);
   }

   // display your name, command line args, time
   time(&timer);      // get time
   printf("Patrick Hughes     %s %s     %s", 
           argv[0], argv[1], asctime(localtime(&timer)));

   infile = fopen(argv[1], "rb"); // open file in binary mode
   if (!infile)
   {
      printf("Cannot open input file %s\n", argv[1]);
      exit(1);
   }

   fread(&letter, 1, 1, infile);  // test for and discard get file sig
   if (letter != 'o')
   {
      printf("%s not an lcc file", argv[1]);
      exit(1);
   }
   fread(&letter, 1, 1, infile);  // test for and discard 'C'
   if (letter != 'C')
   {
      printf("Missing C header entry in %s\n", argv[1]);
      exit(1);
   }

   fread(mem, 1, sizeof(mem), infile); // read machine code into mem

   while (1)   // This while loop runs the FIDE Cycle
   {
      // For Debugging
      // printf("\t=pc: %d, op: %x=\n",pc,opcode);


      // =Fetch= instruction, load it into ir, and =Increment= the pc
      ir = mem[pc++];                    

      // *** I was having problems with the double bit shifting not properly sign   ***
      // *** extending, so I modified them to mask for the field, and then if the   ***
      // *** leading bit was 1, meaning the number should be negative, I 'OR'ed     ***
      // *** the field with a hex number, adding in the leading ones manualy.       ***
      
      // Isolate the fields of the instruction in the ir
      opcode = ir >> 12;                          // get opcode
      pcoffset9 = imm9 = ir & 0x1ff;              // Masking to isolate the pcoffset9
      if((ir & 0x0100) >> 8) {pcoffset9 = pcoffset9 | 0xfe00;}    
      // pcoffset9 = ir << 7;                               // left justify pcoffset9 field
      // pcoffset9 = imm9 = pcoffset9 >> 7;                 // sign extend and rt justify
      pcoffset11 = ir & 0x7ff;                    // Masking to isolate the pcoffset11
      if((ir & 0x0400) >> 10) {pcoffset11 = pcoffset11 | 0xf800;}
      // pcoffset11 = ir << 5;                              // left justify pcoffset11 field
      // pcoffset11 = ir >> 5;                              // sign extend and rt justify
      imm5 = ir & 0x1f;                           // Masking to isolate imm5
      if((ir & 0x0010) >> 4) {imm5 = imm5 | 0xffe0;}  
      // imm5 = ir << 11;                                   // left justify imm5 field
      // imm5 = ir >> 11;                                   // sign extend and rt justify
      offset6 = ir & 0x3f;                        // Masking to isolate offset6
      if((ir & 0x0020) >> 5) {offset6 = offset6 | 0xffc0;}
      // offset6 = ir << 10;                      // left justify offset6 field
      // offset6 = ir >> 10;                      // sign extend and rt justify
      eopcode = ir & 0x1f;                        // get 5-bit eopcode field    
      trapvec = ir & 0xff;                        // get 8-bit trapvec field 
      code = dr = sr = (ir & 0x0e00) >> 9;        // get code/dr/sr and rt justify
      sr1 = baser = (ir & 0x01c0) >> 6;           // get sr1/baser and rt justify
      sr2 = ir & 0x7;                             // get third reg field
      bit5 = (ir & 0x0020) >> 5;                  // get bit 5
      bit11 = (ir & 0x0800) >> 11;                // get bit 11

      // =Decode= and =Execute= instruction just fetched
      // printf("%d\n",opcode);
      switch (opcode)
      {
         case 0:                                // branch instructions
            // printf("branch code: %x",code);
            switch(code)
            {
               case 0: if (z == 1)              // brz
                     pc = pc + pcoffset9;
                     break;
               case 1: if (z == 0)              // brnz
                     pc = pc + pcoffset9;
                     break;
               case 2: if (n == 1)              // brn
                     pc = pc + pcoffset9;
                     break;
               case 3: if (n == z)              // brp
                     pc = pc + pcoffset9;
                     break;

               // code missing here

               case 7: 
                  pc = pc + pcoffset9;          // br
                  break;
               default:
                  printf("Somethign is really breaking down...");
            }                                                   
            break;
         case 1:                                // add
            // Debugging
            // printf("ir: %x\n",ir);
            // printf("Opcode:\t%x\n",opcode);
            // printf("dr:\t%x\n",dr);
            // printf("r[dr]:\t%x\n",r[dr]);
            // printf("sr1:\t%x\n",sr1);
            // printf("bit5:\t%x\n",bit5);
            // printf("imm5:\t%x\n",imm5);
            if (bit5)
            {
               regsave1 = r[sr1];
               r[dr] = regsave1 + imm5;
               // set c, v flags
               setcv(r[dr], regsave1, imm5);
            }
            else
            {
               regsave1 = r[sr1]; regsave2 = r[sr2];
               r[dr] = regsave1 + regsave2;
               // set c, v flags
               setcv(r[dr], regsave1, regsave2);
            }
            // set n, z flags
            setnz(r[dr]);
            break;
         case 2:                                // ld
            r[dr] = mem[pc + pcoffset9];
            break;
         case 3:                                // st
            mem[pc + pcoffset9] = r[sr];
            break;
         case 4:
            r[7] = pc;
            if(bit11)                           // bl ,call,jsr
               pc = pc + pcoffset11;
            else                                // blr ,jsrr
               pc = r[baser] +offset6;
            break;
         case 5:                                // and
            // For Debugging
            // printf("ir: %x\n",ir);
            // printf("bit5:\t%x\n",bit5);
            // printf("sr1:\t%x, %d\n",sr1,r[sr1]);
            // printf("sr2:\t%x, %d\n",sr2,r[sr2]);
            // printf("imm5:\t%x\n",imm5);
            if(bit5) {  // add register to imm5
               r[dr] = r[sr1] & imm5;
            } else {    // add register to register
               r[dr] = r[sr1] & r[sr2];
            }
            setnz(r[dr]);
            break;
         case 6:                                // ldr
            r[dr] = mem[r[baser] + offset6];
            break;
         case 7:                                // str
            mem[r[baser] + offset6] = r[sr];
            break;

            // code missing here

         case 9:                                // not
            // ~ is the not operator in C
            r[dr] = ~r[sr1];
            // set n, z flags
            setnz(r[dr]);
            break;

         // code missing here

         case 12:                               // jmp/ret
            // Debugging 
            // printf("ir: %x\n",ir);
            // printf("baser:\t%d\n",baser);
            // printf("r[baser]:\t%d\n",r[baser]);
            // printf("offset6:\t%d\n",offset6);
            pc = r[baser] + offset6;
            break;

         // code missing here

         case 14:                               // lea
            r[dr] = pc + pcoffset9;
            break;
         case 15:                               // trap
            if (trapvec == 0x00)                // halt
               exit(0);
            else
            if (trapvec == 0x01)                // nl
               printf("\n");
            else
            if (trapvec == 0x02)                // dout
               printf("%d",r[sr]);                
            break;
      }
   }
}
