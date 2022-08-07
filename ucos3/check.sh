
# Generate compilation database
CodeChecker log -o ./compilation_database.json -b "make"

# Dump AST
CodeChecker analyze ./compilation_database.json -o CSA-output --ctu-collect --ctu-ast-mode load-from-pch

# Check
CodeChecker analyze ./compilation_database.json --analyzers clangsa \
  --ctu-analyze --ctu-ast-mode load-from-pch \
  --enable-all \
  -o CSA-output \
  --capture-analysis-output 


# Generate reports
CodeChecker parse --print-steps ./CSA-output
CodeChecker parse --export html --output ./reports_html ./CSA-output
