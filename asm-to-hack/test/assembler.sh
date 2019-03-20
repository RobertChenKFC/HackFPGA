for fullFileName in *.asm; do
    printf "\n"

    echo "Assembling $fullFileName"
    ../build/HackAssembler $fullFileName

    fileName="${fullFileName%.*}"
    outputFileName="${fileName}.hack"
    compareFileName="${fileName}_.hack"

    diff $outputFileName $compareFileName
done
