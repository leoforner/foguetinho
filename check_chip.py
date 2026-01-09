import sys
import subprocess
Import("env")

# --- MAC ADDRESSES ---
# IMPORTANTE: Mantenha estes valores em SINCRONIA com o arquivo:
# lib/RocketShared/include/mac_addresses.h

MAC_FLIGHT = "A1:B2:C3:D4:E5:F6"
MAC_GROUND = "11:22:33:44:55:66"

def check_mac(source, target, env):
    print("\n>>> [SECURITY CHECK] Validando Identidade do Chip...")
    try:
        # Usa o esptool para ler o MAC do chip conectado via USB
        ret = subprocess.check_output(["esptool.py", "read_mac"], stderr=subprocess.STDOUT).decode()
        
        current_mac = ""
        for line in ret.split('\n'):
            if "MAC:" in line:
                current_mac = line.split("MAC: ")[1].strip().upper()
                break
        
        env_name = env["PIOENV"]
        
        if env_name == "flight_ctrl" and current_mac != MAC_FLIGHT:
            print(f"\n[ERRO FATAL] Tentando enviar codigo de VOO para o chip errado ({current_mac})!")
            sys.exit(1)
            
        if env_name == "ground_station" and current_mac != MAC_GROUND:
            print(f"\n[ERRO FATAL] Tentando enviar codigo de BASE para o chip errado ({current_mac})!")
            sys.exit(1)
            
        print(f">>> Identidade Confirmada: {current_mac} para {env_name}\n")
            
    except Exception as e:
        print(f"Aviso: Nao foi possivel verificar o MAC. Erro: {e}")

env.AddPreAction("upload", check_mac)