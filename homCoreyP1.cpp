#include <iostream>
#include <string>
#include <fstream>
using namespace std;

//Assume that the initial state of all prediction counters is T
//0 means predicted incorrectly
//1 means predicted correctly
int predictorSingleBit(int * table, int index, char branch)
{
  int correct = 0;
  //if the branch is true
  if (branch == 'T')
  {
    //if >= 1 in the index which we initialized, then that means we predicted correctly
    if (table[index] >= 1)
    {
      correct = 1;
    }
    //if we didn't predict true, we have to increment this index for future
    else
    {
      table[index] += 1;
    }
  }
  else
  {
    //if not taken, and we have it predicted to be not taken, we are correct
    if (table[index] < 1)
    {
      correct = 1;
    }
    //if not taken but predicted to be taken, we decrement this index for the future
    else
    {
      table[index] -= 1;
    }
  }
  return correct;
}

//assume TT
//we can index % 4 to get the state since there 
int predictor2Bit(int *table, int index, char branch)
{
    int correct = 0;
    int state = table[index] % 4;
    if (branch == 'T') {
      //NT or weakly NT just increment index for next time
      if (state == 0)
      {
        table[index]++;
      }
      else if (state == 1)
      {
          table[index]++;
      }
      //if TT or weak T we are 1
      else if (state == 2)
      {
          table[index]++;
          correct = 1;
      }
      else if (state == 3)
      {
          correct = 1;
      }
    }
    else
    {
      //other way around for NT
        if (state == 0)
        {
            correct = 1;
        }
        else if (state == 1)
        {
            correct = 1;
            table[index]--;
        }
        else if (state == 2)
        {
            table[index]--;
        }
        else if (state == 3)
        {
            table[index]--;
        }
    }
    return correct;
}
int main ( int argc, char *argv[] )
{
  if ( argc != 3 ) 
    cout<<"usage: "<< argv[0] <<" <file input> " << "<file output>\n";
  else {
    ifstream input( argv[1] );
    if ( !input.is_open() )
      cout<<"Could not open file\n";
    //file is open now.
    else 
    {
      //Open the output file
      ofstream output (argv[2]);
      
      //Bimodal predictors need a table of size 16, 32, 128, 256, 512, 1024, 2048
      //Note there is no size 64
      int *tableS16 = new int[16];
      int *tableS32 = new int[32];
      int *tableS128 = new int[128];
      int *tableS256 = new int[256];
      int *tableS512 = new int[512];
      int *tableS1024 = new int[1024];
      int *tableS2048 = new int[2048];
      int *tableD16 = new int[16];
      int *tableD32 = new int[32];
      int *tableD128 = new int[128];
      int *tableD256 = new int[256];
      int *tableD512 = new int[512];
      int *tableD1024 = new int[1024];
      int *tableD2048 = new int[2048];
      
      //Prediction table for bimodal table
      for (int i = 0; i < 16; i++)
      {
        tableS16[i] = 1;
        tableD16[i] = 3;
      }
      for (int i = 0; i < 32; i++)
      {
        tableS32[i] = 1;
        tableD32[i] = 3;
      }
      for (int i = 0; i < 128; i++)
      {
        tableS128[i] = 1;
        tableD128[i] = 3;
      }
      for (int i = 0; i < 256; i++)
      {
        tableS256[i] = 1;
        tableD256[i] = 3;
      }
      for (int i = 0; i < 512; i++)
      {
        tableS512[i] = 1;
        tableD512[i] = 3;
      }
      for (int i = 0; i < 1024; i++)
      {
        tableS1024[i] = 1;
        tableD1024[i] = 3;
      }
      for (int i = 0; i < 2048; i++)
      {
        tableS2048[i] = 1;
        tableD2048[i] = 3;
      }
      //Counters
      int alwaysTaken = 0;
      int alwaysNot = 0;
      int GHR = 0;
      int single16 = 0, single32 = 0, 
      single128 = 0, single256 = 0, 
      single512 = 0, single1024 = 0, single2048 = 0;
      int two16 = 0, two32 = 0, 
      two128 = 0, two256 = 0, 
      two512 = 0, two1024 = 0, two2048 = 0;
      //Read the file
      while (!input.eof())
      {
        long long address;
        //we can convert the address to hex using std::hex
        input >> hex >> address;
        if (input.fail()) break;

        char branch;
        input >> branch;
        if (input.fail()) break;

        input.ignore(256, '\n');
        
        //GHR T = 1
        //Part 1 and 2 are covered here
        if (branch == 'T')
        {
          alwaysTaken++;
          GHR++;
        }
        else
        {
          alwaysNot++;
        }
        
        //Part 3
        //Small memory that is indexed by the lower portion of the address of the branch instruction
        //Contains one or more bits indicating whether the branch was recently taken or not
        single16 += predictorSingleBit(tableS16, address % 16, branch);
        single32 += predictorSingleBit(tableS32, address % 32, branch);
        single128 += predictorSingleBit(tableS128, address % 128, branch);
        single256 += predictorSingleBit(tableS256, address % 256, branch);
        single512 += predictorSingleBit(tableS512, address % 512, branch);
        single1024 += predictorSingleBit(tableS1024, address % 1024, branch);
        single2048 += predictorSingleBit(tableS2048, address % 2048, branch);
        
        two16 += predictor2Bit(tableD16, address % 16, branch);
        two32 += predictor2Bit(tableD32, address % 32, branch);
        two128 += predictor2Bit(tableD128, address % 128, branch);
        two256 += predictor2Bit(tableD256, address % 256, branch);
        two512 += predictor2Bit(tableD512, address % 512, branch);
        two1024 += predictor2Bit(tableD1024, address % 1024, branch);
        two2048 += predictor2Bit(tableD2048, address % 2048, branch);
      }
      cout << "alwaysTaken: " << alwaysTaken << endl;
      cout << "alwaysNot: " << alwaysNot << endl;
      cout << "Total: " << alwaysTaken + alwaysNot << endl;
      cout << "single16: " << single16 << endl;
      cout << "single32: " << single32 << endl;
      cout << "single128: " << single128 << endl;
      cout << "single256: " << single256 << endl;
      cout << "single512: " << single512 << endl;
      cout << "single1024: " << single1024 << endl;
      cout << "single2048: " << single2048 << endl;
      cout << "two16: " << two16 << endl;
      cout << "two32: " << two32 << endl;
      cout << "two128: " << two128 << endl;
      cout << "two256: " << two256 << endl;
      cout << "two512: " << two512 << endl;
      cout << "two1024: " << two1024 << endl;
      cout << "two2048: " << two2048 << endl;
      output.close();
      //Remember to delete the tables
      delete [] tableS16;
      delete [] tableS32;
      delete [] tableS128;
      delete [] tableS256;
      delete [] tableS512;
      delete [] tableS1024;
      delete [] tableS2048;
      delete [] tableD16;
      delete [] tableD32;
      delete [] tableD128;
      delete [] tableD256;
      delete [] tableD512;
      delete [] tableS1024;
      delete [] tableS2048;
    }
    input.close();
  }
}
