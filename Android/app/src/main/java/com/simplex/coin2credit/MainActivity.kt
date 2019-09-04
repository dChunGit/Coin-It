package com.simplex.coin2credit

import android.app.Activity
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.os.Bundle
import com.google.android.material.navigation.NavigationView
import androidx.core.view.GravityCompat
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AppCompatActivity
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import com.simplex.coin2credit.database.*
import com.simplex.coin2credit.ndef.NdefReader
import com.simplex.coin2credit.ndef.NdefWriter
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.app_bar_main.*
import kotlinx.android.synthetic.main.content_main.*
import uk.co.chrisjenx.calligraphy.CalligraphyConfig
import uk.co.chrisjenx.calligraphy.CalligraphyContextWrapper

/**
 * Quick sample client for Coin-It embedded nfc wireless credit
 * TODO: Move strings to constants
 * TODO: Rx for database reports and nfc reads
 * TODO: Replace asynctask with coroutines for async
 * TODO: Inject db
 * TODO: Abstract current state and cache values
 */
class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener, TransactionInterface {
    private var state = NFCState.CONNECTED //state 0 -> write connected, state 1 -> read amount -> write done
    private lateinit var tag: Tag
    private var message = "Confirmed:David"
    private var error = 0
    private var currentTotal = 0
    private var currentTransaction = 0
    private var read = false
    private val moneyFormat = "$%d.%d"

    private lateinit var database: AppDatabase
    private lateinit var databaseValues: List<Transaction>

    private var mNfcAdapter: NfcAdapter? = null
    private val mimeText = "text/plain"

    override fun attachBaseContext(newBase: Context?) {
        super.attachBaseContext(CalligraphyContextWrapper.wrap(newBase))
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        CalligraphyConfig.initDefault(CalligraphyConfig.Builder()
                .setDefaultFontPath("fonts/ARCENA.ttf")
                .setFontAttrId(R.attr.fontPath)
                .build()
        )

        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)

        val toggle = ActionBarDrawerToggle(this, drawer_layout, toolbar,
                R.string.navigation_drawer_open,
                R.string.navigation_drawer_close)
        drawer_layout.addDrawerListener(toggle)
        toggle.syncState()

        nav_view.setNavigationItemSelectedListener(this)

        mNfcAdapter = NfcAdapter.getDefaultAdapter(this)
        Toast.makeText(this, if (mNfcAdapter == null) "This device doesn't support NFC" else "NFC disabled", Toast.LENGTH_LONG).show()

        // for testing only, should put into keystore
        database = AppDatabase.getAppDatabase(this, charArrayOf('t', 'e', 's', 't', 'i', 'n', 'g'))
        AsyncGet(database, this).execute()
        handleIntent(intent)
    }

    override fun onSaveInstanceState(outState: Bundle?) {
        super.onSaveInstanceState(outState)
        outState?.apply {
            putSerializable("State", state)
            putInt("Error", error)
        }
    }

    override fun onRestoreInstanceState(savedInstanceState: Bundle?) {
        super.onRestoreInstanceState(savedInstanceState)
        savedInstanceState?.apply {
            if (getInt("Error", 0) < 3) {
                state = getSerializable("State") as NFCState
                message = when (state) {
                    NFCState.CONNECTED -> "Done"
                    NFCState.COMPLETE -> "Confirmed:David"
                }
            }
        }
    }

    override fun onBackPressed() = drawer_layout.takeIf { it.isDrawerOpen(GravityCompat.START) }?.closeDrawer(GravityCompat.START)
                ?: super.onBackPressed()

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean = when (item.itemId) {
            R.id.action_settings -> true
            else -> super.onOptionsItemSelected(item)
        }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.nav_home -> {}
            R.id.nav_recent -> {}
            R.id.nav_settings -> {}
            R.id.nav_about -> {}
        }

        drawer_layout.closeDrawer(GravityCompat.START)
        return true
    }

    override fun onNewIntent(intent: Intent) = handleIntent(intent)

    private fun handleIntent(intent: Intent) {
        val action = intent.action
        if (NfcAdapter.ACTION_NDEF_DISCOVERED == action) {
            if (mimeText == intent.type) {
                tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
                Log.d(LOG_TAG, "Current State: $state")
                when (state) {
                    NFCState.CONNECTED -> NdefWriter(this, message).execute(tag)
                    NFCState.COMPLETE -> NdefReader(this).execute(tag)
                }

            } else {
                Log.d(LOG_TAG, "Wrong mime: " + intent.type)
            }
        }
    }

    override fun onResume() {
        super.onResume()
        setupForegroundDispatch(this, mNfcAdapter)
    }

    override fun onPause() {
        super.onPause()
        stopForegroundDispatch(this, mNfcAdapter)
    }

    private fun setupForegroundDispatch(activity: Activity, adapter: NfcAdapter?) {
        val intent = Intent(activity.applicationContext, activity.javaClass)
        intent.flags = Intent.FLAG_ACTIVITY_SINGLE_TOP

        val pendingIntent = PendingIntent.getActivity(activity.applicationContext, 0, intent, 0)

        val filters = arrayOfNulls<IntentFilter>(1)
        filters[0] = IntentFilter().apply {
            addAction(NfcAdapter.ACTION_NDEF_DISCOVERED)
            addCategory(Intent.CATEGORY_DEFAULT)
            addDataType(mimeText)
        }

        adapter?.enableForegroundDispatch(activity, pendingIntent, filters, arrayOf<Array<String>>())
    }

    private fun stopForegroundDispatch(activity: Activity, adapter: NfcAdapter?) {
        adapter?.disableForegroundDispatch(activity)
    }

    override fun databaseRead(transactions: List<Transaction>) {
        databaseValues = transactions
    }

    override fun messageWritten(success: Boolean) {
        if (success) {
            when (state) {
                NFCState.CONNECTED -> {
                    message = "Done"
                    state = NFCState.COMPLETE
                }
                NFCState.COMPLETE -> {
                    message = "Confirmed:David"
                    state = NFCState.CONNECTED
                    read = false
                }
            }
            Log.d(LOG_TAG, "State change $state")
        } else {
            Log.d(LOG_TAG, "Message Write Failed")
            error++
            if (error < 3) { NdefWriter(this, message).execute(tag) } else Log.d(LOG_TAG, "Retries failed")
        }
    }

    override fun relayMessage(data: String) {
        if (!data.contains("Confirmed") && data != "Done" && !read) {
            read = true
            currentTransaction++
            AsyncStore(database).execute(data)
            data.toInt().let {
                currentTotal += it
                lastNum.text = moneyFormat.formatMoney(it)
                currentBal.text = moneyFormat.formatMoney(currentTotal)

                NdefWriter(this, message).execute(tag)
                val transactionFragment = TransactionFragment.newInstance(it)
                transactionFragment.show(supportFragmentManager, TRANSACTION_TAG)
                Log.d(LOG_TAG, "State Change $state $message")
            }
        } else {
            NdefReader(this).execute(tag)
        }
    }

    private fun String.formatMoney(value: Int) = format(this, value/100, value%100)

    enum class NFCState() {
        CONNECTED, COMPLETE
    }
    
    companion object {
        const val LOG_TAG = "TAG"
        const val TRANSACTION_TAG = "TransactionDialog"
    }
}
