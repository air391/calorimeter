cat B4_nt_B4_t0.csv | grep '#' > B4.csv
cat B4_nt_B4_t*.csv | grep -v '#' >> B4.csv
