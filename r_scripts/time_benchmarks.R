library(tidyverse)

adapted_insert <- "tables/benchmark_adapted_insert.csv"
adapted_query_negative <- "tables/benchmark_adapted_query_negative.csv"
adapted_query_positive <- "tables/benchmark_adapted_query_positive.csv"

bloom_insert <- "tables/benchmark_bloom_insert.csv"
bloom_query_negative <- "tables/benchmark_bloom_query_negative.csv"
bloom_query_positive <- "tables/benchmark_bloom_query_positive.csv"

table <- read_csv(adapted_insert)


table <- table |>
  mutate(time_in_s = time / 10**9)


graph <- table |>
  ggplot(aes(x = name, y=time_in_s)) +
  scale_y_continuous(limits = c(0, 35)) +
  geom_col() +
  labs(x = "Filter", y = "Time (s)") 
  
graph


