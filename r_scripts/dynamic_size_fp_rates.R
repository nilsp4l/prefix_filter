library(tidyverse)

simd <- read_csv("tables/dynamic_size_fp_rates_simd.csv")
adapted <- read_csv("tables/dynamic_size_fp_rates_adapted.csv")

simd <- simd |>
  mutate(size_in_mbytes=size / 10**6) |>
  mutate(fp_rate_in_percent= fp_rate*100)

adapted <- adapted |>
  mutate(size_in_mbytes=size / 10**6) |>
  mutate(fp_rate_in_percent= fp_rate*100)

all <- rbind(simd, adapted)

graph <- ggplot(all, aes(x = size_in_mbytes, y = fp_rate_in_percent, group=name, color=name)) +
  geom_point() + geom_line() +
  labs(y="FP-Rate (%)", x="Size (MB)", color="Filter")

graph
