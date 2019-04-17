for projectDir in */; do
    for category in ${projectDir}*; do
        for testCase in ${category}/*; do
            for fullFileName in ${testCase}/*.vm; do
                ../build/VMTranslator fullFileName.vm
            done
        done
    done
done
