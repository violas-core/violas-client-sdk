# Generate

target/release/generate-transaction-builders \
    --language cpp \
    --module-name diem_framework \
    --with-diem-types "testsuite/generate-format/tests/staged/diem.yaml" \
    --target-source-dir target/release/cpp \
    "language/diem-framework/releases/legacy" \
    "language/diem-framework/releases/artifacts/current"