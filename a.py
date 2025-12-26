#!/usr/bin/env python3
import glob, re

logs = glob.glob('output_*.log')
games = positions = gps = pps = tsec = 0
ppg_list = []

for f in logs:
    try:
        with open(f) as file:
            lines = [re.sub(r'\x1b\[[0-9;]*[a-zA-Z]', '', l) for l in file.readlines()]
            # Get last 6 non-empty lines that contain metrics
            metrics = []
            for line in reversed(lines):
                line = line.strip()
                if line and any(x in line for x in ['Games played:', 'Positions saved:', 'Positions per game:', 'Total time:', 'Games per sec:', 'Positions per sec:']):
                    metrics.insert(0, line)
                    if len(metrics) == 6:
                        break
            
            if len(metrics) == 6:
                # Parse metrics
                games += int(re.search(r'Games played:\s*(\d+)', metrics[0]).group(1))
                positions += int(re.search(r'Positions saved:\s*(\d+)', metrics[1]).group(1))
                ppg_list.append(int(re.search(r'Positions per game:\s*(\d+)', metrics[2]).group(1)))
                
                t = re.search(r'Total time:\s*(\d+)h\s*(\d+)m\s*(\d+)s', metrics[3])
                h, m, s = map(int, t.groups())
                tsec += h*3600 + m*60 + s
                
                gps += float(re.search(r'Games per sec:\s*([\d.]+)', metrics[4]).group(1))
                pps += float(re.search(r'Positions per sec:\s*([\d.]+)', metrics[5]).group(1))
                
    except:
        continue

mean_ppg = sum(ppg_list)/len(ppg_list) if ppg_list else 0
h, m, s = tsec//3600, (tsec%3600)//60, tsec%60

print(f"Games played: {games}")
print(f"Positions saved: {positions}")
print(f"Positions per game: {mean_ppg:.1f}")
print(f"Total time: {h}h {m}m {s}s")
print(f"Games per sec: {gps:.3f}")
print(f"Positions per sec: {pps:.3f}")
