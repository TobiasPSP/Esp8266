
# url for our webservice running on ESP8266 (make sure you adjust the IP address to match yours):
$urlInternal = 'http://192.168.68.125/led'

$text = Read-Host -Prompt 'Enter your text to convert to morse code'

$ProgressPreference = 'SilentlyContinue'


# convert text to morse code
Add-Type -AssemblyName System.Web
$encoded = [System.Web.HttpUtility]::UrlEncode($Text)

# compose web service URL (NOTE THERE ARE JUST 10 FREE REQUESTS TO THIS WEBSERVICE PER HOUR)
$urlExternal = "https://api.funtranslations.com/translate/morse.json?text=$encoded"

# call web service
$morse = (Invoke-RestMethod -UseBasicParsing -Uri $urlExternal).contents.translated

$morse

# send morse code to ESP8266


$timing = @{
    '.' = {
            $null = Invoke-WebRequest -UseBasicParsing -Uri "$urlInternal/on"
            Start-Sleep -Milliseconds 50
            $null = Invoke-WebRequest -UseBasicParsing -Uri "$urlInternal/off"
            Start-Sleep -Milliseconds 200
          }
    '-' = {
            $null = Invoke-WebRequest -UseBasicParsing -Uri "$urlInternal/on"
            Start-Sleep -Milliseconds 500
            $null = Invoke-WebRequest -UseBasicParsing -Uri "$urlInternal/off"
            Start-Sleep -Milliseconds 200
          }
    ' ' = { Start-Sleep -Milliseconds 500 }
}


$morse.ToCharArray() | 
    ForEach-Object { 
        Write-Host $_ -NoNewline
        & $timing[[string]$_] 
        
    } 
