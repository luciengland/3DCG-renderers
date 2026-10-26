/// "Import 3D Objects in C++" by EETechStuff on YouTube
/// Part 1: https://www.youtube.com/watch?v=3bf1jmoQTOo
/// Part 2: https://www.youtube.com/watch?v=lisdh3J7Np0
/// "Reading & Writing Files in C++"
/// https://www.youtube.com/watch?v=kmGT5DjuTdk


#include <iostream>
#include <fstream> /// ifstream, ofstream
#include <vector>
#include <string>
#include <sstream>
#include <iomanip> /// to set cout precision
using namespace std;

/// class that stores vector of ints
class Face
{
public:
    std::vector<uint32_t> VertexIndices{};
    std::vector<uint32_t> TextureIndices{};
    std::vector<uint32_t> NormalIndices{};
};

/// ------------------------------- INITIALIZE --------------------------------


/// final output for OBJ file numerical vectors
std::vector<double> xVals;
std::vector<double> yVals;

/// final output for OBJ file numerical vectors
std::vector<std::vector<float>> VertexPositions;
std::vector<std::vector<float>> TextureCoordinates;
std::vector<std::vector<float>> NormalVectors;
std::vector<std::vector<uint32_t>> Indices;
std::vector<Face> Faces;

/// --------------------------------- METHODS ---------------------------------

// void FileOpenError(const std::string& path)
// {
// }

/// string methods
std::vector<std::string> ReadFromTextFile(const std::string& path)
{
    /// hold contents of CSV file as single long string
    std::string totalCSVContentsString;
    /// define object of "ostringstream" class called myStringStream
    std::ostringstream myStringStream = std::ostringstream{};
    /// make new ifstream object associated with "path" and open
    std::ifstream inputFile(path);
    if (!inputFile.is_open())
    {
        // FileOpenError(path);
    }

    /// obtain pointer to string buffer for myStringStream and read into buffer
    /// myStringStream uses string buffer that contains sequence of characters
    /// which can be accessed directly as string object
    myStringStream << inputFile.rdbuf();
    /// take ostringstream and make into string
    totalCSVContentsString = myStringStream.str();
    /// convert string into input string stream so getline() can work on it
    /// define input string stream using just-read array of characters
    std::istringstream myCSVStringStream(totalCSVContentsString);
    std::vector<std::string> records;
    std::string record;
    /// extracts complete line, reading from array of characters until
    /// newline encountered, and stores it in string "record"
    while (std::getline(myCSVStringStream, record))
    {
        records.push_back(record);
    }
    std::cout << "Number of records in " << path << " = " << records.size() << std::endl << std::endl;
    return records;
}

// void PrintVecString(std::vector<std::string> const& input)
// {
// }

/// csv methods
// void WriteToCSV(const std::string& path, std::string outString)
// {
// }

// void ParseCSVtoDouble(std::vector<std::string> csvRecordStrings)
// {
// }

/// obj methods
std::vector<uint32_t> ParseFaceIndices(std::string indexGroup)  /// "#/#/#"
{
    std::string value;
    std::vector<uint32_t> indices;
    /// parse string and convert to std::vector<uint32_t> with 3 elements
    std::stringstream instream(indexGroup);
    while (std::getline(instream, value, '/'))
    {
        indices.push_back(std::stoi(value));
    }
    return indices;
}

void ParseOBJ(std::vector<std::string> OBJRecordStrings)
{
    /// internal counters 
    std::vector<uint32_t> VertsPerFace; /// to determine if triangles or quads
    uint32_t vertsPerFace = 0;          /// each face, how many vertices it has
    uint32_t numFaces = 0;              /// track number of faces                  
    /// intermediate string values
    std::string value;                  /// "throw away" values during parsing
    std::vector<std::string> values;
    /// intermediate/individual numerical values
    std::vector<float> VertexPosition;  /// for lines starting with "v"
    std::vector<float> TextureCoordinate; /// for lines starting with "vt"
    std::vector<float> NormalVector; /// for lines starting with "vn"

    /// parse values from OBJ file 
    for (int i = 0; i < OBJRecordStrings.size(); i++)
    {
        std::stringstream sstream(OBJRecordStrings[i]);
        /// check if starting character is "v" for each line
        while (std::getline(sstream, value, ' '))
        {
            /// if start with "v", parse string values & put in vector<string>
            if (value == "v")
            {
                /// parse 3 values 
                for (int j = 0; j < 3; j++)
                {
                    std::getline(sstream, value, ' ');
                    float valFloat = std::stof(value);
                    VertexPosition.push_back(valFloat);
                }
                VertexPositions.push_back(VertexPosition);
            }
            /// parse 2 values 
            if (value == "vt")
            {
                for (int j = 0; j < 2; j++)
                {
                    std::getline(sstream, value, ' ');
                    float valFloat = std::stof(value);
                    TextureCoordinate.push_back(valFloat);
                }
                TextureCoordinates.push_back(TextureCoordinate);
            }
            if (value == "vn")
            {
                for (int j = 0; j < 3; j++)
                {
                    std::getline(sstream, value, ' ');
                    float valFloat = std::stof(value);
                    NormalVector.push_back(valFloat);
                }
                NormalVectors.push_back(NormalVector);
            }
            if (value == "f")
            {
                /// use newline character to get entire line and return as value
                while (std::getline(sstream, value))
                {
                    /// convert value into string stream
                    std::stringstream facestream(value);
                    /// parse line based on space delimiters
                    while (std::getline(facestream, value, ' '))
                    {
                        Indices.push_back(ParseFaceIndices(value));
                        vertsPerFace++;
                    }
                    VertsPerFace.push_back(vertsPerFace);
                    vertsPerFace = 0;
                }
                numFaces++;
            }
        }
    }
    /// take all Indices elements and create individual face components
    /// b/c num verts can vary for each face, refer to VertsPerFace
    /// vector which keeps track of number of verts defined in a line
    uint32_t k = 0;
    uint32_t j = 0;

    for (uint32_t i = 0; i < numFaces; i++)
    {
        Face* face = new Face();
        while (j < VertsPerFace[i])
        {
            face->VertexIndices.push_back(Indices[k][0]);
            face->TextureIndices.push_back(Indices[k][1]);
            face->NormalIndices.push_back(Indices[k][2]);
            /// increment num verts defined in each line
            j++;
            k++;
        }
        j = 0;
        Faces.push_back(*face);
    }
} /// end ParseOBJ

// void PrintOBJData()
// {
//     
// }

/// ---------------------------------- MAIN -----------------------------------

int main()
{
    /// define file
    std::string filename("ex.obj");
    /// define vector of strings to hold each line of file
    std::vector<std::string> RecordsStr;

    /// get a vector of strings, each string reps each line in file
    RecordsStr = ReadFromTextFile(filename);

    /// extracting the last 3 characters of file name
    std::string fileSuffix = filename.substr(filename.length() - 3, filename.length());

    /// if OBJ file
    if (fileSuffix == "obj")
    {
        ParseOBJ(RecordsStr);
        // PrintOBJData();
    }
    /// if CSV file
    else if (fileSuffix == "csv")
    {
        // ParseCSVtoDouble(RecordsStr);
        for (int i=0; i < xVals.size(); i++)
        {
            std::cout << "X[" << i << "] = "  << yVals[i] << 
                        " Y [" << i << "] = " << yVals[i] << std::endl;
        }
    }
    /// any other file suffix
    else
    {
        std::cout << "Error while reading '" << fileSuffix << "' file." << std::endl;
        // PrintVecString(RecordsStr);
    }

    std::cin.get(); 
}

/// ---------------------------------------------------------------------------

