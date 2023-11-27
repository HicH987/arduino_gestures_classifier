import csv
import os


def csv_to_list(f_name):
    str_lst = lambda string, split_char: list(string.split(split_char))

    data = []
    rows = []
    nb_gest = 0
    nb_rows = 0

    with open(f_name, "r") as file:
        for row in file:
            if not "#" in row:
                lst_row = str_lst(row.strip(), ",")
                rows.append(lst_row)
            else:
                data.append(rows)
                nb_rows += len(rows)
                nb_gest += 1
                rows = []

    return data, nb_gest, nb_rows


def merge_file(f_merge_name, data_1, data_2, csv_head):
    nb_rows = 0
    with open(f_merge_name, "w", newline="") as f_merge:
        merge_csv = csv.writer(f_merge)
        merge_csv.writerow(csv_head)

        for d1, d2 in zip(data_1, data_2):
            merge_csv.writerows(d1)
            merge_csv.writerows(d2)
            merge_csv.writerow([])
            nb_rows = nb_rows + len(d1) + len(d2)

    print(f"- '{f_merge_name}' was created !")

    return nb_rows


def merging_csv(f_name1, f_name2, f_merge_name):
    csv_head = ["aX", "aY", "aZ", "gX", "gY", "gZ"]

    data_1, nb_gest1, nbr_rows1 = csv_to_list(f_name1)
    data_2, nb_gest2, nbr_rows2 = csv_to_list(f_name2)

    nb_rows_mrg = merge_file(f_merge_name, data_1, data_2, csv_head)

    print(
        f"""---------------- '{f_merge_name}' LOG -------------------------------
    |               FILE_NAME      |   NB_GEST |   NB_ROWS
    |_________________________________________________________________
    |               {os.path.basename(f_name1)}       |   {nb_gest1}      |   {nbr_rows1}
    |               {os.path.basename(f_name2)}       |   {nb_gest2}      |   {nbr_rows2}
    |---MERGE--->   {os.path.basename(f_merge_name)}      |   {nb_gest1}      |   {nb_rows_mrg}
---------------------------------------------------------------\n"""
    )


