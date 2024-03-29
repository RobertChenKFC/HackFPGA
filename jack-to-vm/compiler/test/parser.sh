echo Building 10/Square...
../build/JackCompiler 10/Square/Main.jack 10/Square/Square.jack 10/Square/SquareGame.jack --parse
echo Comparing 10/Square/Main.xml...
diff 10/Square/Main_.xml 10/Square/Main.xml --strip-trailing-cr
echo Comparing 10/Square/Square.xml...
diff 10/Square/Square_.xml 10/Square/Square.xml --strip-trailing-cr
echo Comparing 10/Square/SquareGame.xml...
diff 10/Square/SquareGame_.xml 10/Square/SquareGame.xml --strip-trailing-cr

printf "\n"
echo Building 10/ExpressionLessSquare...
../build/JackCompiler 10/ExpressionLessSquare/Main.jack 10/ExpressionLessSquare/Square.jack 10/ExpressionLessSquare/SquareGame.jack --parse
echo Comparing 10/ExpressionLessSquare/Main.xml...
diff 10/ExpressionLessSquare/Main_.xml 10/ExpressionLessSquare/Main.xml --strip-trailing-cr
echo Comparing 10/ExpressionLessSquare/Square.xml...
diff 10/ExpressionLessSquare/Square_.xml 10/ExpressionLessSquare/Square.xml --strip-trailing-cr
echo Comparing 10/ExpressionLessSquare/SquareGame.xml...
diff 10/ExpressionLessSquare/SquareGame_.xml 10/ExpressionLessSquare/SquareGame.xml --strip-trailing-cr

printf "\n"
echo Building 10/ArrayTest...
../build/JackCompiler 10/ArrayTest/Main.jack --parse
echo Comparing 10/ArrayTest/Main.xml...
diff 10/ArrayTest/Main_.xml 10/ArrayTest/Main.xml --strip-trailing-cr

