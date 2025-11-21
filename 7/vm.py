import os

def read_memory_values():
    total_vmsize = 0
    total_vmrss = 0
    
    for pid in os.listdir('/proc'):
        if pid.isdigit():
            status_path = f'/proc/{pid}/status'
            try:
                with open(status_path, 'r') as f:
                    for line in f:
                        if line.startswith('VmSize:'):
                            total_vmsize += int(line.split()[1]) 
                        elif line.startswith('VmRSS:'):
                            total_vmrss += int(line.split()[1]) 
            except FileNotFoundError:
                continue
            except PermissionError:
                continue

    return total_vmsize, total_vmrss

vmsize_sum, vmrss_sum = read_memory_values()

print(f"Suma VmSize: {vmsize_sum} kB")
print(f"Suma VmRSS: {vmrss_sum} kB")
