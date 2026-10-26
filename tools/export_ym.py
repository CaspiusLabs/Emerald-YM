"""Export a committed project snapshot and full local Git history. No MIDI/network."""
from pathlib import Path
import argparse,hashlib,subprocess,json
ROOT=Path(__file__).resolve().parents[1]
def git(*args):
    return subprocess.check_output(['git',*args],cwd=ROOT,text=True).strip()
def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--ref',default='v'+(ROOT/'VERSION').read_text().strip())
    ap.add_argument('--output',type=Path,default=ROOT/'dist')
    a=ap.parse_args()
    revision=git('rev-parse','--verify',a.ref+'^{commit}')
    version=git('show',revision+':VERSION').strip()
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    archive=out/f'Emerald-YM-v{version}-source.zip'
    subprocess.run(['git','archive','--format=zip',f'--prefix=Emerald-YM-v{version}/',f'--output={archive}',revision],cwd=ROOT,check=True)
    bundle=out/'Emerald-YM-history.bundle'
    subprocess.run(['git','bundle','create',str(bundle),'--all'],cwd=ROOT,check=True)
    subprocess.run(['git','bundle','verify',str(bundle)],cwd=ROOT,check=True)
    manifest={'ref':a.ref,'commit':revision,'version':version,'source_archive':archive.name,
              'git_bundle':bundle.name,'runtime_included':False,'files':{}}
    for p in (archive,bundle):
        manifest['files'][p.name]={'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()}
    (out/'project-export.json').write_text(json.dumps(manifest,indent=2)+'\n')
    (out/'SHA256SUMS.txt').write_text('\n'.join(v['sha256']+'  '+k for k,v in manifest['files'].items())+'\n')
    print(json.dumps(manifest,indent=2))
if __name__=='__main__':main()
