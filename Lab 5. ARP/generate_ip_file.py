with open("ip.list", "w") as file:
    for i1 in range(192,193):
        for i2 in range(168,169):
            for i3 in range(0,256):
                for i4 in range(0,256):
                    file.write(f"{i1}.{i2}.{i3}.{i4}\n")
    file.close()