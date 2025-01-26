library(tidyverse)

fp_rates_table <- read_csv("tables/fp_rates.csv")


fp_rates_table <- fp_rates_table |>
  mutate(size_in_mbytes=size_in_bytes / 10**6) |>
  mutate(spare_size_in_mbytes= spare_size_in_bytes/10**6) |>
  mutate(fp_rate_in_percent= fp_rate*100)

fp_bar_graph <- fp_rates_table |>
  ggplot(aes(x = name, y=size_in_mbytes)) +
  geom_col() +
  geom_col(aes(x = name, y = spare_size_in_mbytes), fill="brown1") +
  geom_text(aes(label=paste(fp_rate_in_percent, "%")), position=position_stack(vjust=.8), color="white") +
  labs(x = "Filter", y = "Size in MB") 

fp_bar_graph
