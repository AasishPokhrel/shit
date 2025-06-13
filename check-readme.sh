#!/bin/bash

if grep -iq "shit" README.md; then
  echo "✅ README contains 'shit'"
  exit 0
else
  echo "❌ README does not contain 'shit'"
  exit 1
fi
