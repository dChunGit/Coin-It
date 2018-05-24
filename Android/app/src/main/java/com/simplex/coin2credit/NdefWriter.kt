package com.simplex.coin2credit

import android.nfc.NdefMessage
import android.nfc.NdefRecord
import android.nfc.Tag
import android.nfc.tech.Ndef
import android.nfc.tech.NdefFormatable
import android.os.AsyncTask
import android.util.Log
import java.io.ByteArrayOutputStream
import java.io.IOException
import java.nio.charset.Charset
import java.util.*

class NdefWriter(context: TransactionInterface, data: String): AsyncTask<Tag, Void, Int>() {
    private var mContext = context
    private var mData = data

    override fun doInBackground(vararg p0: Tag?): Int {
        Log.d("DEBUG", "WRITING " + mData)
        val tag = p0[0] as Tag
        val content = mData.toByteArray(Charset.forName("UTF-8"))
        val lang = Locale.getDefault().language.toByteArray(Charset.forName("UTF-8"))

        val langSize = lang.size
        val contentSize = content.size

        val payload = ByteArrayOutputStream(1 + langSize + contentSize)
        payload.write((langSize and 0x1F))
        payload.write(lang, 0, langSize)
        payload.write(content, 0, contentSize)

        //val ndefRecord = NdefRecord.createMime("text/plain", payload)
        val ndefRecord = NdefRecord(NdefRecord.TNF_WELL_KNOWN,  //Our 3-bit Type name format
                NdefRecord.RTD_TEXT,        //Description of our payload
                ByteArray(0),                //T
                // he optional id for our Record
                payload.toByteArray())
        val nfcMessage = NdefMessage(arrayOf(ndefRecord))

        try {
            val nDef = Ndef.get(tag)

            nDef?.let {
                nDef.connect()
                if (nDef.maxSize < nfcMessage.toByteArray().size) {
                    //Message too large to write to NFC tag
                    Log.d("DEBUG","Too large")
                    return 0
                }
                when(nDef.isWritable) {
                    true -> {
                        nDef.writeNdefMessage(nfcMessage)
                        nDef.close()
                        //Message is written to tag
                        return 1
                    }
                    false -> {
                        Log.d("DEBUG", "Not writable")
                        return 0
                    }
                }
            }

            Log.d("DEBUG", "formatting")
            val nDefFormatableTag = NdefFormatable.get(tag)

            nDefFormatableTag.let {
                try {
                    it.connect()
                    it.format(nfcMessage)
                    it.close()
                    //The data is written to the tag
                    return 1
                } catch (e: IOException) {
                    //Failed to format tag or Ndef not supported
                    Log.d("DEBUG", "Not format")
                    return 0
                }
            }

        } catch (e: Exception) {
            //Write operation has failed
            e.printStackTrace()
        }

        Log.d("DEBUG", "Something else")
        return 0
    }

    override fun onPostExecute(result: Int?) {
        if(result != null) {
            mContext.messageWritten(result)
            //Toast.makeText(mContext, "Content: " + result, Toast.LENGTH_LONG).show()
        }
    }

}
